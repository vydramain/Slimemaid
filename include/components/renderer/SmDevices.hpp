/*
------------------------------------
  Slimemaid Source Code (22.05.2022)
  This file is part of Slimemaid Source Code.
  Vulkan separates the concept of physical and logical devices.
  A physical logical_device usually represents a single complete implementation of Vulkan
  (excluding instance-level functionality) available to the host, of which there are a finite number.
  A logical logical_device represents an instance of that implementation with its own state
  and resources independent of other logical devices.
------------------------------------
*/

#ifndef SLIMEMAID_SMDEVICES_HPP
#define SLIMEMAID_SMDEVICES_HPP

#include <vulkan/vulkan.h>

struct SmDevices {
  VkPhysicalDevice physical_device;
  VkDevice logical_device;

  SmDevices() {
    physical_device = VK_NULL_HANDLE;
    logical_device = VK_NULL_HANDLE;
  }
};

#endif  // SLIMEMAID_SMDEVICES_HPP
