/*
------------------------------------
  Slimemaid Source Code (15.06.2022)
  This file is part of Slimemaid Source Code.

------------------------------------
*/

#include "systems/renderer/SmGraphicsMemorySystem.hpp"

uint32_t find_memory_type(SmDevices input_devices,
                          uint32_t input_type_filter,
                          VkMemoryPropertyFlags input_properties) {
  VkPhysicalDeviceMemoryProperties device_memory_properties;
  vkGetPhysicalDeviceMemoryProperties(input_devices.physical_device,
                                      &device_memory_properties);

  for (uint32_t i = 0; i < device_memory_properties.memoryTypeCount; i++) {
    if (input_type_filter & (1 << i) && (device_memory_properties.memoryTypes[i].propertyFlags & input_properties)) {
      return i;
    }
  }

  throw std::runtime_error("Failed to find suitable memory type");
}
