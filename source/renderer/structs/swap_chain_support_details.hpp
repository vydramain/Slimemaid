/*
------------------------------------

Slimemaid Source Code (30.04.2022)

This file is part of Slimemaid Source Code.

This file has heading of allocation functions for Vulkan API.

------------------------------------
*/

#ifndef SM_VULKAN_SWAP_CHAIN_SUPPORT_DETAILS_
#define SM_VULKAN_SWAP_CHAIN_SUPPORT_DETAILS_

#include <vector>

#include <vulkan/vulkan.h>

/*
------------
Struct keep information about device capabilities on initialized surface.
------------
*/
struct swap_chain_support_details {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

/*
------------
Function query device capabilities on initialized surface.
Formats and presentModes can be loaded only if `inputDevice` and `inputSurface` was declared before function start.
------------
*/
swap_chain_support_details querySwapChainSupport(VkPhysicalDevice inputDevice, VkSurfaceKHR inputSurface) {
  swap_chain_support_details details;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(inputDevice, inputSurface, &details.capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(inputDevice, inputSurface, &formatCount, nullptr);
  if (0 != formatCount) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(inputDevice, inputSurface, &formatCount, details.formats.data());
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(inputDevice, inputSurface, &presentModeCount, nullptr);

  if (0 != presentModeCount) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(inputDevice,
                                              inputSurface,
                                              &presentModeCount,
                                              details.presentModes.data());
  }

  return details;
}

#endif
