
#include "systems/renderer/SmDeviceSystem.hpp"

#include <cstdint>  // Necessary for uint32_t
#include <iostream>
#include <set>
#include <stdexcept>
#include <vector>

#include <vulkan/vulkan.h>

#include "components/renderer/SmDevices.hpp"
#include "components/renderer/SmQueues.hpp"
#include "components/renderer/SmSamplingFlags.hpp"
#include "components/renderer/SmVulkanInstance.hpp"

#include "systems/renderer/SmSwapChainSystem.hpp"
#include "systems/renderer/SmQueueFamiliesSystem.hpp"
#include "systems/renderer/SmSamplingFlagsSystem.hpp"

bool check_device_extension_support(VkPhysicalDevice input_device,
                                    std::vector<const char*>& device_extensions) {
  uint32_t extension_count;
  vkEnumerateDeviceExtensionProperties(input_device,
                                       nullptr,
                                       &extension_count,
                                       nullptr);

  std::vector<VkExtensionProperties> available_extensions(extension_count);
  vkEnumerateDeviceExtensionProperties(input_device,
                                       nullptr,
                                       &extension_count,
                                       available_extensions.data());

  std::set<std::string> required_extensions(device_extensions.begin(), device_extensions.end());

  for (const auto& extension : available_extensions) {
    required_extensions.erase(extension.extensionName);
  }
  return required_extensions.empty();
}

bool is_device_suitable(VkPhysicalDevice input_device,
                        SmSurface input_surface,
                        std::vector<const char*>& device_extensions) {
  SmQueueFamilyIndices tmp_indices = find_transfer_queue_families(input_device, input_surface);
  bool extensions_supported = check_device_extension_support(input_device, device_extensions);
  bool swap_chain_adequate = false;

  if (extensions_supported) {
    SmSwapChainSupportDetails swap_chain_support = query_swap_chain_support(input_device, input_surface);
    swap_chain_adequate = !swap_chain_support.formats.empty() && !swap_chain_support.present_modes.empty();
  }

  VkPhysicalDeviceFeatures supported_features;
  vkGetPhysicalDeviceFeatures(input_device, &supported_features);

  return tmp_indices.is_indices_complete() &&
         extensions_supported &&
         swap_chain_adequate &&
         supported_features.samplerAnisotropy;
}

void pick_physical_device(SmVulkanInstance input_instance,
                          SmSamplingFlags* p_samples_fls,
                          SmDevices* p_devices,
                          SmSurface input_surface,
                          std::vector<const char*>& device_extensions) {
  uint32_t device_count = 0;
  vkEnumeratePhysicalDevices(input_instance.instance,
                             &device_count,
                             nullptr);

  if (0 == device_count) {
    throw std::runtime_error("No GPUs with Vulkan support found");
  }

  std::vector<VkPhysicalDevice> tmp_devices(device_count);
  vkEnumeratePhysicalDevices(input_instance.instance,
                             &device_count,
                             tmp_devices.data());

  for (const auto& tmp_device : tmp_devices) {
    if (is_device_suitable(tmp_device, input_surface, device_extensions)) {
      p_samples_fls->msaa_samples = get_max_usable_sample_count(tmp_device);
      p_devices->physical_device = tmp_device;
      break;
    }
  }

  if (VK_NULL_HANDLE == p_devices->physical_device) {
    throw std::runtime_error("No suitable GPU found");
  }

  VkPhysicalDeviceProperties chosen_device_properties;
  vkGetPhysicalDeviceProperties(p_devices->physical_device, &chosen_device_properties);

  std::cout << "Picking physical devices.devices process ends with success..." << std::endl
            << "Chosen physical device is: " << chosen_device_properties.deviceName << std::endl;
}

void create_logical_device(SmDevices* devices,
                           SmSurface input_surface,
                           SmQueues* queues,
                           bool input_enable_validation_layers,
                           std::vector<const char*>& validation_layers,
                           std::vector<const char*>& device_extensions) {
  SmQueueFamilyIndices transfer_indices = find_transfer_queue_families(devices->physical_device,
                                                                       input_surface);

  std::vector<VkDeviceQueueCreateInfo> transfer_queue_create_infos;
  std::set<uint32_t> transfer_unique_queue_families = {transfer_indices.graphics_family.value(),
                                                       transfer_indices.present_family.value()};

  float transfer_queue_priority = 1.0f;
  for (uint32_t queue_family : transfer_unique_queue_families) {
    VkDeviceQueueCreateInfo transfer_queue_create_info{};
    transfer_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    transfer_queue_create_info.queueFamilyIndex = queue_family;
    transfer_queue_create_info.queueCount = 1;
    transfer_queue_create_info.pQueuePriorities = &transfer_queue_priority;
    transfer_queue_create_infos.push_back(transfer_queue_create_info);
  }

  VkPhysicalDeviceFeatures device_features{};
  device_features.samplerAnisotropy = VK_TRUE;
  device_features.sampleRateShading = VK_TRUE;  // enable sample shading feature for the devices.devices

  VkDeviceCreateInfo device_create_info{};
  device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_create_info.queueCreateInfoCount = static_cast<uint32_t>(transfer_queue_create_infos.size());
  device_create_info.pQueueCreateInfos = transfer_queue_create_infos.data();
  device_create_info.pEnabledFeatures = &device_features;
  device_create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
  device_create_info.ppEnabledExtensionNames = device_extensions.data();

  if (input_enable_validation_layers) {
    device_create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
    device_create_info.ppEnabledLayerNames = validation_layers.data();
  } else {
    device_create_info.enabledLayerCount = 0;
  }

  if (VK_SUCCESS != vkCreateDevice(devices->physical_device,
                                   &device_create_info,
                                   nullptr,
                                   &devices->logical_device)) {
    throw std::runtime_error("Failed to create logical devices.devices");
  }

  vkGetDeviceQueue(devices->logical_device,
                   transfer_indices.graphics_family.value(),
                   0,
                   &queues->graphics_queue);
  vkGetDeviceQueue(devices->logical_device,
                   transfer_indices.present_family.value(),
                   0,
                   &queues->present_queue);

  std::cout << "Logical devices.devices creation process ends with success..." << std::endl;
}
