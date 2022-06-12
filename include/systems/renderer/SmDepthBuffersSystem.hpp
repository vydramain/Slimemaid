/*
------------------------------------
  Slimemaid Source Code (12.06.2022)
  This file is part of Slimemaid Source Code.

------------------------------------
*/

#ifndef SLIMEMAID_SMDEPTHBUFFERSSYSTEM_HPP
#define SLIMEMAID_SMDEPTHBUFFERSSYSTEM_HPP

#include <vulkan/vulkan.h>

VkFormat find_supported_depth_format(SmDevices input_devices,
                                     const std::vector<VkFormat>& candidates,
                                     VkImageTiling input_tiling,
                                     VkFormatFeatureFlags input_flags) {
  for (VkFormat format : candidates) {
    VkFormatProperties device_properties;
    vkGetPhysicalDeviceFormatProperties(input_devices.physical_device,
                                        format,
                                        &device_properties);

    if (VK_IMAGE_TILING_LINEAR == input_tiling && (device_properties.linearTilingFeatures & input_flags) == input_flags) {
      return format;
    } else if (VK_IMAGE_TILING_OPTIMAL == input_tiling &&
               (device_properties.optimalTilingFeatures & input_flags) == input_flags) {
      return format;
    }
  }

  throw std::runtime_error("Failed to find supported depth format");
}

VkFormat find_depth_format(SmDevices input_devices) {
  return find_supported_depth_format(input_devices,
                                     {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                                     VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

#endif  // SLIMEMAID_SMDEPTHBUFFERSSYSTEM_HPP
