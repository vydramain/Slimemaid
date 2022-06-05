/*
------------------------------------
  Slimemaid Source Code (04.06.2022)
  This file is part of Slimemaid Source Code.
  Surface component provides object that provides a function for destroying VkSurfaceKHR objects
  To establish the connection between Vulkan and the window system to present results to the screen,
  use the WSI (Window System Integration) extensions.
------------------------------------
*/

#ifndef SLIMEMAID_SMSURFACE_HPP
#define SLIMEMAID_SMSURFACE_HPP

#include <vulkan/vulkan.h>

struct SmSurface {
  VkSurfaceKHR surface_khr;
};

#endif  // SLIMEMAID_SMSURFACE_HPP
