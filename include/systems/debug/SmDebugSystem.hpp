/*
------------------------------------
  Slimemaid Source Code (07.06.2022)
  This file is part of Slimemaid Source Code.
  The Vulkan API is designed around the idea of minimal driver overhead and one of the manifestations of that
  goal is that there is very limited error checking in the API by default. Even mistakes as simple
  as setting enumerations to incorrect values or passing null pointers to required parameters
  are generally not explicitly handled and will simply result in crashes or undefined behavior.

  For checks this all Vulkan has validation layers. Validation layers is Vulkan system there are
  optional components that hook into Vulkan function calls to apply additional operations.
  Common operations in validation layers are:
    - Checking the values of parameters against the specification to detect misuse
    - Tracking creation and destruction of objects to find resource leaks
    - Checking thread safety by tracking the threads that calls originate from
    - Logging every call and its parameters to the standard output
    - Tracing Vulkan calls for profiling and replaying

  Debug system initialize validation layers and setup debug messenger for them.
------------------------------------
*/

#ifndef SLIMEMAID_SMDEBUGSYSTEM_H
#define SLIMEMAID_SMDEBUGSYSTEM_H

#ifdef NDEBUG
const bool enable_validation_layers = false;
#else
const bool enable_validation_layers = true;
#endif

#include <vulkan/vulkan.h>

const std::vector<const char*> validation_layers = { "VK_LAYER_KHRONOS_validation" };

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                    void* pUserData) {
  std::cerr << pCallbackData->pMessage << '\n';

  return VK_FALSE;
}

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

void fill_debug_messenger_create_info_EXT(VkDebugUtilsMessengerCreateInfoEXT& create_info) {
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

void setup_debug_messenger(bool input_enable_validation_layers,
                           SmVulkanInstance input_instance,
                           VkDebugUtilsMessengerEXT* p_debug_messenger) {
  std::cout << "Enable validation layers flag is: " << enable_validation_layers << std::endl;
  if (!input_enable_validation_layers) return;

  VkDebugUtilsMessengerCreateInfoEXT create_info;
  fill_debug_messenger_create_info_EXT(create_info);

  if (VK_SUCCESS != create_debug_utils_messenger_EXT(
                        input_instance.instance,
                        &create_info,
                        nullptr,
                        p_debug_messenger)) {
    throw std::runtime_error("Failed to set up debug messenger");
  }

  std::cout << "Debug messenger setup process ends with success..." << std::endl;
}

#endif  // SLIMEMAID_SMDEBUGSYSTEM_H
