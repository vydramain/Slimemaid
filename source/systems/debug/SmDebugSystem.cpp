/*
------------------------------------
  Slimemaid Source Code (15.06.2022)
  This file is part of Slimemaid Source Code.

------------------------------------
*/

#include "systems/debug/SmDebugSystem.hpp"

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                     VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                     const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                     void* pUserData) {
  std::cerr << pCallbackData->pMessage << '\n';

  return VK_FALSE;
}

VkResult create_debug_utils_messenger_EXT(VkInstance input_instance,
                                          VkDebugUtilsMessengerCreateInfoEXT* p_create_info,
                                          VkDebugUtilsMessengerEXT* p_debug_messenger,
                                          VkAllocationCallbacks* p_allocator) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(input_instance,
                                                                         "vkCreateDebugUtilsMessengerEXT");
  if (nullptr != func) {
    return func(input_instance, p_create_info, p_allocator, p_debug_messenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void destroy_debug_utils_messenger_EXT(VkInstance input_instance,
                                       VkDebugUtilsMessengerEXT* p_debug_messenger,
                                       VkAllocationCallbacks* p_allocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(input_instance,
                                                                          "vkDestroyDebugUtilsMessengerEXT");
  if (nullptr != func) {
    func(input_instance, *p_debug_messenger, p_allocator);
  }
}

bool check_validation_layer_support() {
  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

  std::vector<VkLayerProperties> available_layers(layer_count);
  vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

  for (const char* layerName : validation_layers) {
    bool layerFound = false;
    for (const auto& layerProperties : available_layers) {
      if (0 == strcmp(layerName, layerProperties.layerName)) {
        layerFound = true;
        break;
      }
    }
    if (!layerFound) {
      return false;
    }
  }

  return true;
}

void fill_debug_messenger_create_info_EXT(VkDebugUtilsMessengerCreateInfoEXT* p_create_info) {
  p_create_info = {};
  p_create_info->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  p_create_info->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  p_create_info->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  p_create_info->pfnUserCallback = debug_callback;
  p_create_info->pUserData = nullptr;  // Optional
}

void setup_debug_messenger(bool input_enable_validation_layers,
                           SmVulkanInstance input_instance,
                           VkDebugUtilsMessengerEXT* p_debug_messenger) {
  std::cout << "Enable validation layers flag is: " << enable_validation_layers << std::endl;
  if (!input_enable_validation_layers) return;

  VkDebugUtilsMessengerCreateInfoEXT create_info;
  fill_debug_messenger_create_info_EXT(&create_info);

  if (VK_SUCCESS != create_debug_utils_messenger_EXT(
                        input_instance.instance,
                        &create_info,
                        p_debug_messenger,
                        nullptr)) {
    throw std::runtime_error("Failed to set up debug messenger");
  }

  std::cout << "Debug messenger setup process ends with success..." << std::endl;
}
