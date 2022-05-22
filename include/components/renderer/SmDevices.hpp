/*
------------------------------------
  Slimemaid Source Code (22.05.2022)
  This file is part of Slimemaid Source Code.
  Component with devices information. Vulkan have physic and virtual devices.
  These devices provide access to memory and commands queue and pools.
------------------------------------
*/

#ifndef SLIMEMAID_SMDEVICES_HPP
#define SLIMEMAID_SMDEVICES_HPP

#include <vulkan/vulkan.h>

struct SmDevices {
  VkPhysicalDevice physical_device;
  VkDevice device;

  SmDevices() {
    physical_device = VK_NULL_HANDLE;
  }
};

#endif  // SLIMEMAID_SMDEVICES_HPP
