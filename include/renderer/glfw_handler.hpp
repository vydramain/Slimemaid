/*
------------------------------------

Slimemaid Source Code (01.05.2022)

This file is part of Slimemaid Source Code.

This file has heading of allocation functions for Vulkan API.

------------------------------------
*/

#ifndef SM_VULKAN_GLFW_HANDLER_
#define SM_VULKAN_GLFW_HANDLER_

#include <iostream>

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "consts.hpp"

class glfw_handler {
private:
    GLFWwindow *window;

    void initWindow(GLFWframebuffersizefun callback);
public:
    glfw_handler(GLFWframebuffersizefun inputCallback);
    ~glfw_handler();

    void glfwHandleWaitEvents();
    bool glfwHandleWindowShouldClose();
    void *glfwHandlerGetWindowUserPointer();
    void glfwHandleGetFramebufferSize(int *height, int *width);
    VkResult glfwHandleCreateWindowSurface(VkInstance inputInstance,
                                           const VkAllocationCallbacks* allocator,
                                           VkSurfaceKHR* surface);
};

#endif
