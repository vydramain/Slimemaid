/*
------------------------------------
  Slimemaid Source Code (04.06.2022)
  This file is part of Slimemaid Source Code.
  The instance is the connection between your application and the Vulkan library
  and creating it involves specifying some details about your application to the driver.
------------------------------------
*/

#ifndef SLIMEMAID_SMVULKANINSTANCE_HPP
#define SLIMEMAID_SMVULKANINSTANCE_HPP

#include <vulkan/vulkan.h>

struct SmVulkanInstance {
  VkInstance instance;
};

#endif  // SLIMEMAID_SMVULKANINSTANCE_HPP
