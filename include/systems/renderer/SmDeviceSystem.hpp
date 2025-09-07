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

#include <vector>

#include <vulkan/vulkan.h>

#include "components/renderer/SmDevices.hpp"
#include "components/renderer/SmSamplingFlags.hpp"
#include "components/renderer/SmSurface.hpp"
#include "components/renderer/SmVulkanInstance.hpp"
#include "components/renderer/SmQueues.hpp"

bool sl_check_device_extension_support(VkPhysicalDevice input_device,
                                    std::vector<const char*>& device_extensions);

bool sl_is_device_suitable(VkPhysicalDevice input_device,
                        SmSurface input_surface,
                        std::vector<const char*>& device_extensions);

void sl_pick_physical_device(SmVulkanInstance input_instance,
                          SmSamplingFlags* p_samples_fls,
                          SmDevices* p_devices,
                          SmSurface input_surface,
                          std::vector<const char*>& device_extensions);

void sl_create_logical_device(SmDevices* devices,
                           SmSurface input_surface,
                           SmQueues* queues,
                           bool input_enable_validation_layers,
                           std::vector<const char*>& validation_layers,
                           std::vector<const char*>& device_extensions);

#endif  // SLIMEMAID_SMDEVICESYSTEM_HPP
