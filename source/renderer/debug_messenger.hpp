/*
------------------------------------

Slimemaid Source Code (28.04.2022)

This file is part of Slimemaid Source Code.

This file has heading of allocation functions for Vulkan API.

------------------------------------
*/

#ifndef SM_VULKAN_DEBUG_MESSENGER_
#define SM_VULKAN_DEBUG_MESSENGER_

#include <stdexcept>
#include <iostream>
#include <cstdio>

#include <vulkan/vulkan.h>


/*
------------
Function gets current Vulkan instance, pointers to debug create info struct, memory allocator, debug messenger as field
for address.
Function returns Vulkan error code or VK result code - "VK_SUCCESS".
------------
*/
VkResult CreateDebugUtilsMessengerEXT(VkInstance inputInstance,
                                      const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                      const VkAllocationCallbacks *pAllocator,
                                      VkDebugUtilsMessengerEXT *pDebugMessenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      inputInstance, "vkCreateDebugUtilsMessengerEXT");
  if (nullptr != func) {
    return func(inputInstance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

/*
------------
Function gets current Vulkan instance, debugger messenger and pointer to memory allocaator.
Function try to free debug messenger instance and its memory.
------------
*/
void DestroyDebugUtilsMessengerEXT(VkInstance inputInstance,
                                   VkDebugUtilsMessengerEXT inputDebugMessenger,
                                   const VkAllocationCallbacks *pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(inputInstance,
                                                                          "vkDestroyDebugUtilsMessengerEXT");
  if (nullptr != func) {
    func(inputInstance, inputDebugMessenger, pAllocator);
  }
}

/*
------------
------------
*/
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                    void *pUserData) {
  std::cerr << pCallbackData->pMessage << '\n';

  return VK_FALSE;
}

/*
------------
------------
*/
void fillDebugMessengerCreateInfoEXT(VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
  createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = debugCallback;
  createInfo.pUserData = nullptr; // Optional
}

#endif
