/*
------------------------------------

Slimemaid Source Code (29.04.2022)

This file is part of Slimemaid Source Code.

This file has heading of allocation functions for Vulkan API.

------------------------------------
*/

#include "renderer/structs/queue_family_indices.hpp"

queue_family_indices findQueueFamilies(VkPhysicalDevice inputDevice, VkSurfaceKHR inputSurface) {
  queue_family_indices indices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(inputDevice, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(inputDevice, &queueFamilyCount, queueFamilies.data());

  int i = 0;
  for (const auto &queueFamily : queueFamilies) {
    if (indices.isComplete()) {
      break;
    }
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
    }
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(inputDevice, i, inputSurface, &presentSupport);
    if (presentSupport) {
      indices.presentFamily = i;
    }

    i++;
  }

  return indices;
}

queue_family_indices findTransferQueueFamilies(VkPhysicalDevice inputDevice,
                                               VkSurfaceKHR inputSurface) {
  queue_family_indices indices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(inputDevice, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(inputDevice, &queueFamilyCount, queueFamilies.data());

  int i = 0;
  for (const auto &queueFamily : queueFamilies) {
    if (indices.isComplete()) {
      break;
    }
    if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) {
      indices.graphicsFamily = i;
    }
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(inputDevice, i, inputSurface, &presentSupport);
    if (presentSupport) {
      indices.presentFamily = i;
    }

    i++;
  }

  return indices;
}

