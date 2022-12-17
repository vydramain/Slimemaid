/*
------------------------------------
  Slimemaid Source Code (20.05.2022)
  This file is part of Slimemaid Source Code.
  Component need for checking swap chain compability with window surface. There are basically three kinds of properties
  need to check:
    - Basic surface capabilities (min/max number of images in swap chain, min/max width and height of images)
    - Surface formats (pixel format, color space)
    - Available presentation modes
------------------------------------
*/

#ifndef SLIMEMAID_SMSWAPCHAINSUPPORTDETAILS_HPP
#define SLIMEMAID_SMSWAPCHAINSUPPORTDETAILS_HPP

#include <vector>

#include <vulkan/vulkan.h>

struct SmSwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> present_modes;
};

#endif  // SLIMEMAID_SMSWAPCHAINSUPPORTDETAILS_HPP
