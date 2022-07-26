/*
 * vulkan.h
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


#ifndef __VULKAN__
#define __VULKAN__

#include <vulkan/vulkan.h>

#define LP_VK_NV_VERSION_MAJOR(version) ((guint32) version >> 22) & 0x3ff
#define LP_VK_NV_VERSION_MINOR(version) ((guint32) version >> 14) & 0x0ff
#define LP_VK_NV_VERSION_PATCH(version) ((guint32) version >>  6) & 0x0ff


typedef enum LPVkVendorId
{
	LP_VK_VENDOR_ID_NVIDIA = 0x10DE,
	LP_VK_VENDOR_ID_INTEL  = 0x8086
} LPVkVendorId;


VkInstance lunionplay_vulkan_create_instance(void);


VkPhysicalDevice
lunionplay_vulkan_get_better_device(VkInstance* inst, uint32_t* count);


char* lunionplay_vulkan_identify_vendor(uint32_t vendorID);


VkApplicationInfo lunionplay_vulkan_init_application_info(void);


#endif
