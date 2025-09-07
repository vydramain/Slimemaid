/*
------------------------------------
  Slimemaid Source Code (22.05.2022)
  This file is part of Slimemaid Source Code.
------------------------------------
*/

#ifndef SLIMEMAID_SMCOMMANDSSYSTEM_HPP
#define SLIMEMAID_SMCOMMANDSSYSTEM_HPP

#include <vulkan/vulkan.h>

#include "components/renderer/SmDevices.hpp"
#include "components/renderer/SmSurface.hpp"
#include "components/renderer/SmCommandPool.hpp"

VkCommandBuffer sl_begin_single_time_commands(VkDevice input_device,
                                           VkCommandPool input_command_pool);

void sl_end_single_time_commands(VkDevice input_device,
                              VkCommandPool input_command_pool,
                              VkQueue input_graphics_queue,
                              VkCommandBuffer input_command_buffer);

void sl_create_command_pool(SmDevices input_devices,
                       SmSurface input_surface,
                       SmCommandPool* p_command_pool);

void sl_create_command_buffers(SmDevices input_devices,
                          SmCommandPool* p_command_pool);

void sl_create_sync_objects(SmDevices input_devices,
                       SmCommandPool* p_command_pool,
                       std::vector<VkSemaphore>* p_image_available_semaphores,
                       std::vector<VkSemaphore>* p_render_finished_semaphores,
                       std::vector<VkFence>* p_flight_fences);

#endif  // SLIMEMAID_SMCOMMANDSSYSTEM_HPP
