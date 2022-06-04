/*
------------------------------------
  Slimemaid Source Code (22.05.2022)
  This file is part of Slimemaid Source Code.
  SmGLFWWindow component keeps GLFW window.
------------------------------------
*/

#ifndef SLIMEMAID_SMGLFWWINDOW_HPP
#define SLIMEMAID_SMGLFWWINDOW_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

struct SmGLFWWindow {
  GLFWwindow* glfw_window;
};

#endif  // SLIMEMAID_SMGLFWWINDOW_HPP
