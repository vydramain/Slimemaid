/*
------------------------------------

Slimemaid Source Code (01.05.2022)

This file is part of Slimemaid Source Code.

This file has heading of allocation functions for Vulkan API.

------------------------------------
*/

#include "renderer/glfw_handler.hpp"

void glfw_handler::initWindow(GLFWframebuffersizefun inputCallback) {
  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, inputCallback);
  std::cout << "GLFW initialization process ends with success..." << std::endl;
}

glfw_handler::glfw_handler(GLFWframebuffersizefun inputCallback) {
    initWindow(inputCallback);
}

glfw_handler::~glfw_handler() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void glfw_handler::glfwHandleWaitEvents() {
    glfwWaitEvents();
}

bool glfw_handler::glfwHandleWindowShouldClose() {
    return glfwWindowShouldClose(window);
}

void* glfw_handler::glfwHandlerGetWindowUserPointer() {
    return glfwGetWindowUserPointer(window);
}

void glfw_handler::glfwHandleGetFramebufferSize(int* height, int* width) {
    glfwGetFramebufferSize(window, height, width);
}

VkResult glfw_handler::glfwHandleCreateWindowSurface(VkInstance inputInstance,
                                                     const VkAllocationCallbacks* allocator,
                                                     VkSurfaceKHR* surface) {
    return glfwCreateWindowSurface(inputInstance, window, allocator, surface);
}
