
#include "systems/debug/SmDebugSystem.hpp"

#include <vulkan/vulkan.h>

#include <components/renderer/SmVulkanInstance.hpp>
#include <cstring>
#include <iostream>
#include <vector>

static VKAPI_ATTR VkBool32 VKAPI_CALL
debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
               VkDebugUtilsMessageTypeFlagsEXT messageType,
               const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
  std::cerr << pCallbackData->pMessage << std::endl;

  return VK_FALSE;
}

VkResult sl_create_debug_utils_messenger_EXT(VkInstance input_instance,
                                          const VkDebugUtilsMessengerCreateInfoEXT* p_create_info,
                                          const VkAllocationCallbacks* p_allocator,
                                          VkDebugUtilsMessengerEXT* p_debug_messenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      input_instance, "vkCreateDebugUtilsMessengerEXT");
  if (nullptr != func) {
    return func(input_instance, p_create_info, p_allocator, p_debug_messenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void sl_destroy_debug_utils_messenger_EXT(VkInstance input_instance,
                                       VkDebugUtilsMessengerEXT input_debug_messenger,
                                       const VkAllocationCallbacks* p_allocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      input_instance, "vkDestroyDebugUtilsMessengerEXT");
  if (nullptr != func) {
    func(input_instance, input_debug_messenger, p_allocator);
  }
}

bool sl_check_validation_layer_support(std::vector<const char*>* validation_layers) {
  std::cout << "Check Vulkan global layer properties for validation layer support" << std::endl;
  // Retrieving the number of layer properties available is returned in pPropertyCount
  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
  std::cout << "Found `" << layer_count << "` available layer properties" << std::endl;

  // Returns up to requested number of global layer properties
  std::vector<VkLayerProperties> available_layers(layer_count);
  vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

  for (const char* layer_name : *validation_layers) {
    bool layer_found = false;
    for (const auto& layer_properties : available_layers) {
      std::cout << "Found: `" << layer_properties.layerName << "` layer property" << std::endl;
      if (0 == strcmp(layer_name, layer_properties.layerName)) {
        layer_found = true;
        break;
      }
    }

    if (!layer_found) {
      return false;
    }
  }

  return true;
}

void sl_fill_debug_messenger_create_info_EXT(VkDebugUtilsMessengerCreateInfoEXT& create_info) {
  create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  create_info.pfnUserCallback = debug_callback;
  create_info.pUserData = nullptr;  // Optional
}

void sl_setup_debug_messenger(bool input_enable_validation_layers, SmVulkanInstance input_instance,
                           VkDebugUtilsMessengerEXT* p_debug_messenger) {
  std::cout << "Enable validation layers flag is: " << input_enable_validation_layers << std::endl;

  if (!input_enable_validation_layers) return;

  VkDebugUtilsMessengerCreateInfoEXT create_info;
  sl_fill_debug_messenger_create_info_EXT(create_info);

  if (VK_SUCCESS != sl_create_debug_utils_messenger_EXT(input_instance.instance, &create_info, nullptr,
                                                     p_debug_messenger)) {
    throw std::runtime_error("Failed to set up debug messenger");
  }

  std::cout << "Debug messenger setup process ends with success..." << std::endl;
}
