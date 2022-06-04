/*
------------------------------------
  Slimemaid Source Code (04.06.2022)
  This file is part of Slimemaid Source Code.
  Instance object initializes the Vulkan library and
  allows the application to pass information about itself to the implementation.
------------------------------------
*/

#ifndef SLIMEMAID_SMINSTANCE_HPP
#define SLIMEMAID_SMINSTANCE_HPP

#include <vulkan/vulkan.h>

struct SmInstance {
  VkInstance instance;
};

#endif  // SLIMEMAID_SMINSTANCE_HPP
