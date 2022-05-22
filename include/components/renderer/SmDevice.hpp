/*
------------------------------------
  Slimemaid Source Code (22.05.2022)
  This file is part of Slimemaid Source Code.
  Component with device information. Vulkan have physic and virtual devices.
  These devices provide access to memory and commands queue and pools.
------------------------------------
*/

#ifndef SLIMEMAID_SMDEVICE_HPP
#define SLIMEMAID_SMDEVICE_HPP

#include <vulkan/vulkan.h>

struct SmDevice {
  VkPhysicalDevice physical_device;
  VkDevice device;

  SmDevice() {
    physical_device = VK_NULL_HANDLE;
  }
};

#endif  // SLIMEMAID_SMDEVICE_HPP
