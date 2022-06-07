/*
------------------------------------
  Slimemaid Source Code (07.06.2022)
  This file is part of Slimemaid Source Code.

------------------------------------
*/

#ifndef SLIMEMAID_SMDEBUGSYSTEM_H
#define SLIMEMAID_SMDEBUGSYSTEM_H

#include <vulkan/vulkan.h>

VkResult create_debug_utils_messenger_EXT(VkInstance input_instance,
                                      const VkDebugUtilsMessengerCreateInfoEXT* p_create_info,
                                      const VkAllocationCallbacks* p_allocator,
                                      VkDebugUtilsMessengerEXT* p_debug_messenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(input_instance, "vkCreateDebugUtilsMessengerEXT");
  if (nullptr != func) {
    return func(input_instance, p_create_info, p_allocator, p_debug_messenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void destroy_debug_utils_messenger_EXT(VkInstance input_instance,
                                   VkDebugUtilsMessengerEXT input_debug_messenger,
                                   const VkAllocationCallbacks* p_allocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(input_instance, "vkDestroyDebugUtilsMessengerEXT");
  if (nullptr != func) {
    func(input_instance, input_debug_messenger, p_allocator);
  }
}

#endif  // SLIMEMAID_SMDEBUGSYSTEM_H
