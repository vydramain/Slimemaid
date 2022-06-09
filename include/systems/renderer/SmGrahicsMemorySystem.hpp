/*
------------------------------------
  Slimemaid Source Code (22.05.2022)
  This file is part of Slimemaid Source Code.
  Vulkan memory is broken up into two categories, host memory and device memory.
    - Host memory is memory needed by the Vulkan implementation for non-device-visible storage. Vulkan provides
      applications the opportunity to perform host memory allocations on behalf of the Vulkan implementation.
      If this feature is not used, the implementation will perform its own memory allocations. Since most memory
      allocations are off the critical path, this is not meant as a performance feature. Rather, this can be useful
      for certain embedded systems, for debugging purposes (e.g. putting a guard page after all host allocations),
      or for memory allocation logging.
    - Device memory is memory that is visible to the device - for example the contents of the image or buffer objects,
      which can be natively used by the device.
  Graphics memory system seek device memory type.
------------------------------------
*/

#ifndef SLIMEMAID_SMMEMORYTYPEFINDSYSTEM_HPP
#define SLIMEMAID_SMMEMORYTYPEFINDSYSTEM_HPP

#include <vulkan/vulkan.h>

#include <cstdio>
#include <cstdint>  // Necessary for uint32_t
#include <stdexcept>

uint32_t find_memory_type(SmDevices* p_devices,
                          uint32_t input_type_filter,
                          VkMemoryPropertyFlags input_properties) {
  VkPhysicalDeviceMemoryProperties device_memory_properties;
  vkGetPhysicalDeviceMemoryProperties(p_devices->physical_device,
                                      &device_memory_properties);

  for (uint32_t i = 0; i < device_memory_properties.memoryTypeCount; i++) {
    if (input_type_filter & (1 << i) && (device_memory_properties.memoryTypes[i].propertyFlags & input_properties)) {
      return i;
    }
  }

  throw std::runtime_error("Failed to find suitable memory type");
}

#endif  // SLIMEMAID_SMMEMORYTYPEFINDSYSTEM_HPP
