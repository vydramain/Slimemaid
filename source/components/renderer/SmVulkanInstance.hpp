/*
------------------------------------
  Slimemaid Source Code (04.06.2022)
  This file is part of Slimemaid Source Code.
  Instance is  all per-application state in Vulkan. The instance is the connection between your application
  and the Vulkan library.
------------------------------------
*/

#ifndef SLIMEMAID_SMVULKANINSTANCE_HPP
#define SLIMEMAID_SMVULKANINSTANCE_HPP

#include <vulkan/vulkan.h>

struct SmVulkanInstance {
  VkInstance instance;
};

#endif  // SLIMEMAID_SMVULKANINSTANCE_HPP
