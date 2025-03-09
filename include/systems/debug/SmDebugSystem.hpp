/*
------------------------------------
  Slimemaid Source Code (07.06.2022)
  This file is part of Slimemaid Source Code.
  The Vulkan API is designed around the idea of minimal driver overhead and one of the
manifestations of that goal is that there is very limited error checking in the API by default. Even
mistakes as simple as setting enumerations to incorrect values or passing null pointers to required
parameters are generally not explicitly handled and will simply result in crashes or undefined
behavior.

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

#include <vulkan/vulkan.h>

#include <components/renderer/SmVulkanInstance.hpp>
#include <vector>

VkResult create_debug_utils_messenger_EXT(VkInstance input_instance,
                                          const VkDebugUtilsMessengerCreateInfoEXT* p_create_info,
                                          const VkAllocationCallbacks* p_allocator,
                                          VkDebugUtilsMessengerEXT* p_debug_messenger);

void destroy_debug_utils_messenger_EXT(VkInstance input_instance,
                                       VkDebugUtilsMessengerEXT input_debug_messenger,
                                       const VkAllocationCallbacks* p_allocator);

bool check_validation_layer_support(std::vector<const char*>* validation_layers);

void fill_debug_messenger_create_info_EXT(VkDebugUtilsMessengerCreateInfoEXT& create_info);

void setup_debug_messenger(bool input_enable_validation_layers, SmVulkanInstance input_instance,
                           VkDebugUtilsMessengerEXT* p_debug_messenger);

#endif  // SLIMEMAID_SMDEBUGSYSTEM_H
