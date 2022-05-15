/*
------------------------------------

Slimemaid Source Code (25.04.2022)

This file is part of Slimemaid Source Code.

This file has heading of allocation functions for Vulkan API.

------------------------------------
*/

#include "renderer/memory_handler.hpp"

uint32_t findMemoryType(VkPhysicalDevice& inputPhysicalDevice,
                        uint32_t inputTypeFilter,
                        VkMemoryPropertyFlags inputProperties) {
  VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
  vkGetPhysicalDeviceMemoryProperties(inputPhysicalDevice, &deviceMemoryProperties);

  for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
    if (inputTypeFilter & (1 << i) &&
        (deviceMemoryProperties.memoryTypes[i].propertyFlags &
         inputProperties)) {
      return i;
    }
  }

  throw std::runtime_error("Failed to find suitable memory type");
}

