/*
------------------------------------
  Slimemaid Source Code (22.05.2022)
  This file is part of Slimemaid Source Code.
------------------------------------
*/

#ifndef SLIMEMAID_SMSTENCILSYSTEM_HPP
#define SLIMEMAID_SMSTENCILSYSTEM_HPP

#include <vulkan/vulkan.h>

static bool has_stencil_component(VkFormat input_format) {
  return input_format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
         input_format == VK_FORMAT_D24_UNORM_S8_UINT;
}

#endif  // SLIMEMAID_SMSTENCILSYSTEM_HPP
