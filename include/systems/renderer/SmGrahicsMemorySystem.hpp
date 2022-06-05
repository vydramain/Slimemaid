/*
------------------------------------
  Slimemaid Source Code (22.05.2022)
  This file is part of Slimemaid Source Code.
------------------------------------
*/

#ifndef SLIMEMAID_SMMEMORYTYPEFINDSYSTEM_HPP
#define SLIMEMAID_SMMEMORYTYPEFINDSYSTEM_HPP

#include <vulkan/vulkan.h>

#include <cstdio>
#include <cstdint>  // Necessary for uint32_t
#include <stdexcept>

uint32_t findMemoryType(VkPhysicalDevice &physicalDevice,
                        uint32_t inputTypeFilter,
                        VkMemoryPropertyFlags inputProperties) {
  VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);

  for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
    if (inputTypeFilter & (1 << i) && (deviceMemoryProperties.memoryTypes[i].propertyFlags & inputProperties)) {
      return i;
    }
  }

  throw std::runtime_error("Failed to find suitable memory type");
}

#endif  // SLIMEMAID_SMMEMORYTYPEFINDSYSTEM_HPP
