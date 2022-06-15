/*
------------------------------------
  Slimemaid Source Code (22.05.2022)
  This file is part of Slimemaid Source Code.
  GLFW window system provides functions for easy window initialization, termination, control and reload processes.
  Surface is linked with window, so GLFW window system can create surface and set it for window
  for presentation rendered images.
------------------------------------
*/

#ifndef SLIMEMAID_SMGLFWWINDOWSYSTEM_HPP
#define SLIMEMAID_SMGLFWWINDOWSYSTEM_HPP

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include "components/renderer/SmGLFWWindow.hpp"
#include "components/renderer/SmSurface.hpp"
#include "components/renderer/SmVulkanInstance.hpp"

std::vector<const char*> get_required_extensions(bool input_enable_validation_layers);

void create_surface(SmGLFWWindow input_window,
                    SmVulkanInstance input_instance,
                    SmSurface* p_surface);

void get_frame_buffer_size(SmGLFWWindow input_window, int& width, int& height);


#endif  // SLIMEMAID_SMGLFWWINDOWSYSTEM_HPP
