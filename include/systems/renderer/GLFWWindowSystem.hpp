/*
------------------------------------
  Slimemaid Source Code (22.05.2022)
  This file is part of Slimemaid Source Code.
  GLFW window system provides functions for easy window initialization, termination, control and reload processes.
  GLFW window's surface space is also managed by the system.
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

void createSurface(SmGLFWWindow input_window,
                   SmVulkanInstance input_instance,
                   SmSurface& surface) {
  if (VK_SUCCESS != glfwCreateWindowSurface(input_instance.instance,
                                            input_window.glfw_window,
                                            nullptr,
                                            &surface.surface_khr)) {
    throw std::runtime_error("Failed to create window surface_khr");
  }

  std::cout << "GLFW window surface_khr creation process ends with success..." << std::endl;
}

#endif  // SLIMEMAID_GLFWWINDOWSYSTEM_HPP
