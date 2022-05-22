/*
------------------------------------
  Slimemaid Source Code (22.05.2022)
  This file is part of Slimemaid Source Code.
  Window component keeps GLFW window.
------------------------------------
*/

#ifndef SLIMEMAID_WINDOW_HPP
#define SLIMEMAID_WINDOW_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

struct Window {
  GLFWwindow* glfw_window;
};

#endif  // SLIMEMAID_WINDOW_HPP
