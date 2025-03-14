
#include "systems/renderer/SmVulkanInstanceSystem.hpp"

#include <iostream>
#include <stdexcept>

#include "components/renderer/SmVulkanInstance.hpp"

#include "systems/debug/SmDebugSystem.hpp"
#include "systems/renderer/SmGLFWWindowSystem.hpp"

void create_instance(SmVulkanInstance* p_instance,
                    const bool input_enable_validation_layers,
                    std::vector<const char*>* validation_layers) {
  if (!input_enable_validation_layers) {
    throw std::runtime_error("Validation layer are not enabled. Instance can't be created...");
  }

  if (!check_validation_layer_support(validation_layers)) {
    throw std::runtime_error("Validation layers requested, but not avaliable. Instance can't be created...");
  }

  VkApplicationInfo app_info{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "Slimemaid";
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = "No Engine";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo instance_create_info{};
  instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_create_info.pApplicationInfo = &app_info;

  auto glfwExtensions = getRequiredExtensions(input_enable_validation_layers);
  instance_create_info.enabledExtensionCount = static_cast<uint32_t>(glfwExtensions.size());
  instance_create_info.ppEnabledExtensionNames = glfwExtensions.data();

  VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
  if (input_enable_validation_layers) {
    instance_create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers->size());
    instance_create_info.ppEnabledLayerNames = validation_layers->data();

    fill_debug_messenger_create_info_EXT(debug_create_info);
    instance_create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debug_create_info;
  } else {
    instance_create_info.enabledLayerCount = 0;
    instance_create_info.pNext = nullptr;
  }

  if (VK_SUCCESS != vkCreateInstance(&instance_create_info,
                                     nullptr,
                                     &p_instance->instance)) {
    throw std::runtime_error("Failed to create Vulkan p_instance");
  } else {
    std::cout << "Vulkan p_instance creation process ends with success..." << std::endl;
  }
}

