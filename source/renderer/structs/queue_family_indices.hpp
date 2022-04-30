/*
------------------------------------

Slimemaid Source Code (29.04.2022)

This file is part of Slimemaid Source Code.

This file has heading of allocation functions for Vulkan API.

------------------------------------
*/

#ifndef SM_VULKAN_QUEUE_FAMILY_INDICES_STRUCT_
#define SM_VULKAN_QUEUE_FAMILY_INDICES_STRUCT_

#include <cstdint>
#include <optional>
#include <vector>

#include <vulkan/vulkan.h>

/*
------------
Stuct keep uint32_t as optional index of graphics or present family in device.
Struct have optional fields. That mean thad struct can have no values.
------------
*/
struct queue_family_indices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  /*
  ------------
  Method returns boolean value that means that all struct values is exists:
    - <true> All values exists
    - <false> Otherwise.
  ------------
  */
  bool isComplete() {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

/*
------------
Creation function for queue_family_indices.
Create instance with indices to VK_QUEUE_GRAPHICS_BIT and surface present family.
------------
*/
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

/*
------------
Creation function for queue_family_indices.
Create instance with indices to VK_QUEUE_TRANSFER_BIT and surface present family.
------------
*/
queue_family_indices findTransferQueueFamilies(VkPhysicalDevice inputDevice, VkSurfaceKHR inputSurface) {
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

#endif
