/*
------------------------------------
  Slimemaid Source Code (22.05.2022)
  This file is part of Slimemaid Source Code.
------------------------------------
*/

#ifndef SLIMEMAID_SMCOMMANDSSYSTEM_HPP
#define SLIMEMAID_SMCOMMANDSSYSTEM_HPP

#include <vulkan/vulkan.h>

#include <array>
#include <iostream>
#include <vector>

#include "systems/renderer/SmQueueFamiliesSystem.hpp"

#include "components/renderer/SmCommandPool.hpp"
#include "components/renderer/SmDescriptorPool.hpp"
#include "components/renderer/SmDevices.hpp"
#include "components/renderer/SmGraphicsPipeline.hpp"
#include "components/renderer/SmModelResources.hpp"
#include "components/renderer/SmQueueFamilyIndices.hpp"
#include "components/renderer/SmSurface.hpp"
#include "components/renderer/SmSwapChain.hpp"

VkCommandBuffer begin_single_time_commands(VkDevice input_device,
                                           VkCommandPool input_command_pool);

void end_single_time_commands(VkDevice input_device,
                              VkCommandPool input_command_pool,
                              VkQueue input_graphics_queue,
                              VkCommandBuffer input_command_buffer);

void create_command_pool(SmDevices input_devices,
                         SmSurface input_surface,
                         SmCommandPool* p_command_pool);

void create_command_buffers(SmDevices input_devices,
                            SmCommandPool* p_command_pool);

void create_sync_objects(SmDevices input_devices,
                         SmCommandPool* p_command_pool,
                         std::vector<VkSemaphore>* p_image_available_semaphores,
                         std::vector<VkSemaphore>* p_render_finished_semaphores,
                         std::vector<VkFence>* p_flight_fences);

void record_command_buffer(uint32_t image_index,
                           uint32_t input_current_frame_index,
                           SmCommandPool input_command_pool,
                           SmGraphicsPipeline input_graphics_pipeline,
                           SmSwapChain* p_swap_chain,
                           SmDescriptorPool* p_descriptor_pool,
                           SmModelResources* p_model_resources);

#endif  // SLIMEMAID_SMCOMMANDSSYSTEM_HPP
