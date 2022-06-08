/*
------------------------------------
  Slimemaid Source Code (07.06.2022)
  This file is part of Slimemaid Source Code.

------------------------------------
*/

#ifndef SLIMEMAID_SMVULKANINSTANCESYSTEM_H
#define SLIMEMAID_SMVULKANINSTANCESYSTEM_H

#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>

#include "components/renderer/SmVulkanInstance.hpp"
#include "systems/debug/SmDebugSystem.hpp"
#include "systems/renderer/SmGLFWWindowSystem.hpp"

void createInstance(SmVulkanInstance& p_instance,
                    const bool input_enable_validation_layers) {
  if (input_enable_validation_layers && !check_validation_layer_support()) {
    throw std::runtime_error("Validation layers requested, but not avaliable");
  }

  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Slimemaid";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "No Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo instanceCreateInfo{};
  instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceCreateInfo.pApplicationInfo = &appInfo;

  auto glfwExtensions = getRequiredExtensions(input_enable_validation_layers);
  instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(glfwExtensions.size());
  instanceCreateInfo.ppEnabledExtensionNames = glfwExtensions.data();

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
  if (input_enable_validation_layers) {
    instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
    instanceCreateInfo.ppEnabledLayerNames = validation_layers.data();

    fill_debug_messenger_create_info_EXT(debugCreateInfo);
    instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
  } else {
    instanceCreateInfo.enabledLayerCount = 0;
    instanceCreateInfo.pNext = nullptr;
  }

  if (VK_SUCCESS != vkCreateInstance(&instanceCreateInfo,
                                     nullptr,
                                     &p_instance.instance)) {
    throw std::runtime_error("Failed to create Vulkan p_instance");
  } else {
    std::cout << "Vulkan p_instance creation process ends with success..." << std::endl;
  }
}

#endif  // SLIMEMAID_SMVULKANINSTANCESYSTEM_H
