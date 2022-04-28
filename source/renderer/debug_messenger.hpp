/*
------------------------------------

Slimemaid Source Code (28.04.2022)

This file is part of Slimemaid Source Code.

This file has heading of allocation functions for Vulkan API.

------------------------------------
*/

#ifndef SM_VULKAN_DEBUG_MESSENGER_
#define SM_VULKAN_DEBUG_MESSENGER_

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

#endif
