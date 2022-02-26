/*
 * driver.c
 * Copyright (C) 2022 IroAlexis <iroalexis@outlook.fr>
 *
 * lunionplay is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * lunionplay is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "driver.h"

#include "debug.h"

#include <vulkan/vulkan.h>


#define TYPE "driver"


static gchar* lunionplay_driver_get_vendor(guint32 vendorID);


static VkInstance lunionplay_driver_create_vkinstance()
{
	const gchar layer_list[][VK_MAX_EXTENSION_NAME_SIZE] =
	{
		"VK_LAYER_KHRONOS_validation"
	};
	const gchar* layers[] = {
		layer_list[0],
		NULL
	};
	guint32 extension_number = 1;
	const gchar* extensions[] =
	{
		"VK_KHR_surface",
		NULL
	};
	VkApplicationInfo app_info =
	{
		VK_STRUCTURE_TYPE_APPLICATION_INFO,  // VkStructureType sType
		NULL,                                // const void*     pNext
		"LunionPlay",                        // const char*     pApplicationName
		VK_MAKE_VERSION(0, 4, 0),            // uint32_t        applicationVersion
		"NoEngine",                          // const char*     pEngineName
		VK_MAKE_VERSION(1, 0, 0),            // uint32_t        engineVersion
		VK_API_VERSION_1_0,                  // uint32_t        apiVersion
	};
	VkInstanceCreateInfo inst_info =
	{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		NULL,
		VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
		&app_info,
		(uint32_t) 1,
		layers,
		extension_number,
		extensions
	};

	VkInstance inst = VK_NULL_HANDLE;
	VkResult result;

	result = vkCreateInstance(&inst_info , NULL, &inst);

	switch (result)
	{
		case VK_ERROR_INITIALIZATION_FAILED:
			ERR(TYPE, "Failed to create Vulkan instance\n");
			break;
		case VK_ERROR_LAYER_NOT_PRESENT:
			ERR(TYPE, "Layer not present\n");
			break;
		case VK_ERROR_INCOMPATIBLE_DRIVER:
			ERR(TYPE, "Driver not compatible\n");
			break;
		default:
			break;
	}

	return inst;
}


// TODO We cover only one gpu actually...
static VkPhysicalDevice* lunionplay_driver_get_vkphysicaldevice(VkInstance* inst, guint32* number)
{
	VkPhysicalDevice* dev = VK_NULL_HANDLE;

	vkEnumeratePhysicalDevices(*inst, number, VK_NULL_HANDLE);
	TRACE(__FILE__, __func__, "deviceNumber %u\n", *number);

	dev = (VkPhysicalDevice*) g_malloc0(*number * sizeof(VkPhysicalDevice));
	vkEnumeratePhysicalDevices(*inst, number, dev);

	return dev;
}


static LunionPlayDriver* lunionplay_driver_new(VkPhysicalDeviceProperties* props)
{
	LunionPlayDriver* self = NULL;

	self = (LunionPlayDriver*) g_malloc0(sizeof(LunionPlayDriver));
	if (self != NULL)
	{
		self->versionID = props->driverVersion;
		self->vendorID = props->vendorID;
		self->deviceID = props->deviceID;
		self->vendorName = lunionplay_driver_get_vendor(self->vendorID);
		self->deviceName = g_strdup(props->deviceName);

		if (g_strcmp0(self->vendorName, "nvidia") == 0)
		{
			self->version = g_strdup_printf(
				"%u.%u.%u",
				(props->driverVersion >> 22) & 0x3ff,
				(props->driverVersion >> 14) & 0x0ff,
				(props->driverVersion >>  6) & 0x0ff
			);
		}
		else
		{
			self->version = g_strdup_printf(
				"%u.%u.%u",
				VK_VERSION_MAJOR(props->driverVersion),
				VK_VERSION_MINOR(props->driverVersion),
				VK_VERSION_PATCH(props->driverVersion)
			);
		}
	}

	return self;
}


static gchar* lunionplay_driver_get_vendor(guint32 vendorID)
{
	gchar* vendor = NULL;

	switch(vendorID)
	{
		case VK_VENDOR_ID_MESA:
			vendor = g_strdup("mesa");
			break;
		case 0x10DE:
			vendor = g_strdup("nvidia");
			break;
	}

	return vendor;
}


void lunionplay_driver_get(void)
{
	guint32 number = 0;
	VkInstance inst = VK_NULL_HANDLE;
	VkPhysicalDevice* dev = VK_NULL_HANDLE;
	VkPhysicalDeviceProperties* props = VK_NULL_HANDLE;
	LunionPlayDriver* self = NULL;

	inst = lunionplay_driver_create_vkinstance();
	dev = lunionplay_driver_get_vkphysicaldevice(&inst, &number);

	props = (VkPhysicalDeviceProperties*) g_malloc0(sizeof(VkPhysicalDeviceProperties));
	vkGetPhysicalDeviceProperties(*dev, props);

	self = lunionplay_driver_new(props);

	free(props);
	free(dev);
	vkDestroyInstance(inst, NULL);

	TRACE(__FILE__, __func__, "device: %s\n", self->deviceName);
	TRACE(__FILE__, __func__, "vendor: %s\n", self->vendorName);
	TRACE(__FILE__, __func__, "driver: %s\n", self->version);

	lunionplay_driver_free(self);
}


void lunionplay_driver_free(LunionPlayDriver* self)
{
	g_assert(self != NULL);

	g_free(self->version);
	g_free(self->deviceName);
	g_free(self->vendorName);

	g_free(self);
	self = NULL;
}
