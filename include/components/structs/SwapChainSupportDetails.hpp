/*
------------------------------------
  Slimemaid Source Code (20.05.2022)
  This file is part of Slimemaid Source Code.
  Struct SwapChainSupportDetails keeps information about vectors of
  VkSurfaceFormatKHR and VkPresentModeKHR objects.
  This objects keeps swap chain information of current render primitives.
------------------------------------
*/

#ifndef SLIMEMAID_SWAPCHAINSUPPORTDETAILS_HPP
#define SLIMEMAID_SWAPCHAINSUPPORTDETAILS_HPP

#include <vector>

#include <vulkan/vulkan.h>

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

#endif // SLIMEMAID_SWAPCHAINSUPPORTDETAILS_HPP

