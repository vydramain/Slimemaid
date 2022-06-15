/*
------------------------------------
 Slimemaid Source Code (15.06.2022)
 This file is part of Slimemaid Source Code.
----------------------------------
*/

#include "systems/renderer/SmGLFWWindowSystem.hpp"

std::vector<const char*> get_required_extensions(bool input_enable_validation_layers) {
  uint32_t glfw_extensions_count = 0;
  const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extensions_count);

  std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extensions_count);

  if (input_enable_validation_layers) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return extensions;
}

void create_surface(SmGLFWWindow input_window,
                    SmVulkanInstance input_instance,
                    SmSurface* p_surface) {
  if (VK_SUCCESS != glfwCreateWindowSurface(input_instance.instance,
                                            input_window.glfw_window,
                                            nullptr,
                                            &p_surface->surface_khr)) {
    throw std::runtime_error("Failed to create window surface_khr");
  }

  std::cout << "GLFW window surface_khr creation process ends with success..." << std::endl;
}

void get_frame_buffer_size(SmGLFWWindow input_window, int& width, int& height) {
  glfwGetFramebufferSize(input_window.glfw_window, &width, &height);
}
