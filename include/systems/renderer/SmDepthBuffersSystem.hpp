/*
------------------------------------
  Slimemaid Source Code (12.06.2022)
  This file is part of Slimemaid Source Code.

------------------------------------
*/

#ifndef SLIMEMAID_SMDEPTHBUFFERSSYSTEM_HPP
#define SLIMEMAID_SMDEPTHBUFFERSSYSTEM_HPP

#include <vulkan/vulkan.h>

#include "components/renderer/SmQueues.hpp"
#include "components/renderer/SmDevices.hpp"
#include "components/renderer/SmSwapChain.hpp"
#include "components/renderer/SmCommandPool.hpp"
#include "components/renderer/SmDepthBuffers.hpp"
#include "components/renderer/SmSamplingFlags.hpp"

VkFormat sl_find_supported_depth_format(SmDevices input_devices,
                                     const std::vector<VkFormat>& candidates,
                                     VkImageTiling input_tiling,
                                     VkFormatFeatureFlags input_flags);

VkFormat sl_find_depth_format(SmDevices input_devices);

void sl_create_depth_resources(SmDevices input_devices,
                          SmQueues input_queues,
                          SmSamplingFlags input_msaa_samples,
                          SmSwapChain* p_swap_chain,
                          SmCommandPool* p_command_pool,
                          SmDepthBuffers* p_depth_buffers);

#endif  // SLIMEMAID_SMDEPTHBUFFERSSYSTEM_HPP
