/*
------------------------------------
  Slimemaid Source Code (09.06.2022)
  This file is part of Slimemaid Source Code.
  Queue families system get access to family queue indices. Indices point to queue members. Queue member is sequence of
  steps of commands that device should execute.
------------------------------------
*/

#ifndef SLIMEMAID_SMQUEUEFAMILIESSYSTEM_HPP
#define SLIMEMAID_SMQUEUEFAMILIESSYSTEM_HPP

#include <vulkan/vulkan.h>

#include <vector>

#include "components/renderer/SmDevices.hpp"
#include "components/renderer/SmSurface.hpp"
#include "components/renderer/SmQueueFamilyIndices.hpp"


SmQueueFamilyIndices find_transfer_queue_families(VkPhysicalDevice input_device,
                                                  SmSurface input_surface) {
  SmQueueFamilyIndices indices;

  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(input_device,
                                           &queue_family_count,
                                           nullptr);

  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(input_device,
                                           &queue_family_count,
                                           queue_families.data());

  int i = 0;
  for (const auto& queue_family : queue_families) {
    if (indices.is_indices_complete()) {
      break;
    }
    if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphics_family = i;
    }
    VkBool32 present_support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(input_device,
                                         i,
                                         input_surface.surface_khr,
                                         &present_support);
    if (present_support) {
      indices.present_family = i;
    }

    i++;
  }

  return indices;
}

#endif  // SLIMEMAID_SMQUEUEFAMILIESSYSTEM_HPP
