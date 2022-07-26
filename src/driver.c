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
#include "api/vulkan.h"


#define TYPE "driver"


static LunionPlayDriver*
lunionplay_driver_new(const guint32 driverVersion,
                      const guint32 vendorID,
                      const guint32 deviceID,
                      const gchar* deviceName)
{
	LunionPlayDriver* self = NULL;

	self = (LunionPlayDriver*) g_malloc0(sizeof(LunionPlayDriver));
	if (self != NULL)
	{
		self->versionID = driverVersion;
		self->deviceID = deviceID;
		self->deviceName = g_strdup(deviceName);
		self->vendorName = lunionplay_vulkan_identify_vendor(vendorID);
		if (vendorID == 0x10DE)
		{
			self->version = g_strdup_printf(
				"%u.%u.%u",
				LP_VK_NV_VERSION_MAJOR(driverVersion),
				LP_VK_NV_VERSION_MINOR(driverVersion),
				LP_VK_NV_VERSION_PATCH(driverVersion)
			);
		}
		else if (vendorID != 0)
		{
			self->version = g_strdup_printf(
				"%u.%u.%u",
				VK_VERSION_MAJOR(driverVersion),
				VK_VERSION_MINOR(driverVersion),
				VK_VERSION_PATCH(driverVersion)
			);
		}
		else self->version = g_strndup("n/a", 3);
	}

	return self;
}


gboolean lunionplay_driver_get_info_via_vulkan(LunionPlayDriver** self)
{
	guint32 count = 0;
	VkInstance inst;
	VkPhysicalDevice dev;
	VkPhysicalDeviceProperties props;

	inst = lunionplay_vulkan_create_instance();
	dev = lunionplay_vulkan_get_better_device(&inst, &count);
	TRACE(__FILE__, __func__, "deviceNumber %u\n", count);

	vkGetPhysicalDeviceProperties(dev, &props);
	*self = lunionplay_driver_new(props.driverVersion,
	                              props.vendorID,
	                              props.deviceID,
	                              props.deviceName);

	vkDestroyInstance(inst, NULL);

	return TRUE;
}


LunionPlayDriver* lunionplay_driver_create(void)
{
	LunionPlayDriver* self = NULL;

	lunionplay_driver_get_info_via_vulkan(&self);
	if (NULL == self)
	{
		//lunionplay_driver_get_via_opengl(self);
	}

	TRACE(__FILE__, __func__, "device: %s\n", self->deviceName);
	TRACE(__FILE__, __func__, "driver: %s\n", self->version);

	return self;
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
