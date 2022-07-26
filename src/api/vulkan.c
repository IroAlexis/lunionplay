/*
 * vulkan.c
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


#include "vulkan.h"
#include "../debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TYPE "vulkan"


VkInstance lunionplay_vulkan_create_instance(void)
{
	VkApplicationInfo app;
	VkInstanceCreateInfo creatInfo;
	VkInstance inst;
	VkResult res;

	const char layerList[][VK_MAX_EXTENSION_NAME_SIZE] = {"VK_LAYER_KHRONOS_validation"};
	const char* layers[] = {layerList[0]};
	const char* extensions[] = {"VK_KHR_surface"};
	uint32_t layCount = 1;
	uint32_t extCount = 1;

	app = lunionplay_vulkan_init_application_info();

	creatInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	creatInfo.pNext = NULL;
	creatInfo.flags = 0;
	creatInfo.pApplicationInfo = &app;
	creatInfo.enabledLayerCount = layCount;
	creatInfo.ppEnabledLayerNames = layers;
	creatInfo.enabledExtensionCount = extCount;
	creatInfo.ppEnabledExtensionNames = extensions;

	res = vkCreateInstance(&creatInfo , NULL, &inst);
	switch (res)
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
VkPhysicalDevice
lunionplay_vulkan_get_better_device(VkInstance* inst, uint32_t* count)
{
	VkPhysicalDevice dev;

	// Required to count devices before consulting them
	vkEnumeratePhysicalDevices(*inst, count, NULL);

	vkEnumeratePhysicalDevices(*inst, count, &dev);

	return dev;
}


char* lunionplay_vulkan_identify_vendor(uint32_t vendorID)
{
	char* vendor = NULL;

	switch(vendorID)
	{
		case VK_VENDOR_ID_MESA:
			vendor = strdup("mesa");
			break;
		case LP_VK_VENDOR_ID_NVIDIA:
			vendor = strdup("nvidia");
			break;
		case LP_VK_VENDOR_ID_INTEL:
			vendor = strdup("intel");
			break;
	}

	return vendor;
}


VkApplicationInfo lunionplay_vulkan_init_application_info(void)
{
	VkApplicationInfo appInfo;

	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = NULL;
	appInfo.pApplicationName = "LunionPlay";
	appInfo.applicationVersion = VK_MAKE_VERSION(0, 4, 0);
	appInfo.pEngineName = "NoEngine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	return appInfo;
}


