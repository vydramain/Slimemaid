/*
------------------------------------

Slimemaid Source Code (12.05.2022)

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
swap_chain_support_details querySwapChainSupport(VkPhysicalDevice inputDevice,
                                                 VkSurfaceKHR inputSurface);

#endif
