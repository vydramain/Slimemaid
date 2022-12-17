/*
------------------------------------
  Slimemaid Source Code (09.06.2022)
  This file is part of Slimemaid Source Code.
  Device system provides functions for:
    - picking one of graphics card devices that plugged in your system. Physical device picking by special accessible
      features. The features checks in `is_device_suitable` function.
    - creation logical devices. Logical devices creating with present and graphics queues for setting
      commands to execute.
------------------------------------
*/

#ifndef SLIMEMAID_SMDEVICESYSTEM_HPP
#define SLIMEMAID_SMDEVICESYSTEM_HPP

#include <vulkan/vulkan.h>

#include <iostream>
#include <set>
#include <vector>

#include "systems/renderer/SmQueueFamiliesSystem.hpp"
#include "systems/renderer/SmSamplingFlagsSystem.hpp"
#include "systems/renderer/SmSwapChainSystem.hpp"
#include "systems/debug/SmDebugSystem.hpp"

#include "components/renderer/SmDevices.hpp"
#include "components/renderer/SmQueueFamilyIndices.hpp"
#include "components/renderer/SmQueues.hpp"
#include "components/renderer/SmSamplingFlags.hpp"
#include "components/renderer/SmSurface.hpp"
#include "components/renderer/SmSwapChainSupportDetails.hpp"
#include "components/renderer/SmVulkanInstance.hpp"

const std::vector<const char*> device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

bool check_device_extension_support(VkPhysicalDevice input_device);

bool is_device_suitable(VkPhysicalDevice input_device,
                        SmSurface input_surface);

int rate_device_suitability(VkPhysicalDevice input_device);

void pick_physical_device(SmVulkanInstance input_instance,
                          SmSamplingFlags* p_samples_fls,
                          SmDevices* p_devices,
                          SmSurface input_surface);

void create_logical_device(SmDevices* devices,
                           SmSurface input_surface,
                           SmQueues* queues,
                           bool input_enable_validation_layers);

#endif  // SLIMEMAID_SMDEVICESYSTEM_HPP
