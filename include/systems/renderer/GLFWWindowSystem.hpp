/*
------------------------------------
  Slimemaid Source Code (22.05.2022)
  This file is part of Slimemaid Source Code.
  GLFW window system provides functions for easy  window initialization, termination, control and reload processes.
------------------------------------
*/

#ifndef SLIMEMAID_GLFWWINDOWSYSTEM_HPP
#define SLIMEMAID_GLFWWINDOWSYSTEM_HPP

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

#include "systems/debug/SmDebugSystem.hpp"

std::vector<const char*> getRequiredExtensions(bool input_enable_validation_layers) {
  uint32_t glfwExtensionsCount = 0;
  const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

  std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionsCount);

  if (input_enable_validation_layers) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return extensions;
}

#endif  // SLIMEMAID_GLFWWINDOWSYSTEM_HPP
