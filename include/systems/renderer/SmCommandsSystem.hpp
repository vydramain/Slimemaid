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
#include "systems/renderer/SmQueueFamiliesSystem.hpp"

VkCommandBuffer begin_single_time_commands(VkDevice input_device,
                                           VkCommandPool input_command_pool) {
  VkCommandBufferAllocateInfo command_buffer_allocate_info{};
  command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  command_buffer_allocate_info.commandPool = input_command_pool;
  command_buffer_allocate_info.commandBufferCount = 1;

  VkCommandBuffer command_buffer;
  vkAllocateCommandBuffers(input_device,
                           &command_buffer_allocate_info,
                           &command_buffer);

  VkCommandBufferBeginInfo command_buffer_begin_info{};
  command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(command_buffer,
                       &command_buffer_begin_info);

  return command_buffer;
}

void end_single_time_commands(VkDevice input_device,
                              VkCommandPool input_command_pool,
                              VkQueue input_graphics_queue,
                              VkCommandBuffer input_command_buffer) {
  vkEndCommandBuffer(input_command_buffer);

  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &input_command_buffer;

  vkQueueSubmit(input_graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
  vkQueueWaitIdle(input_graphics_queue);

  vkFreeCommandBuffers(input_device,
                       input_command_pool,
                       1,
                       &input_command_buffer);
}

void create_command_pool(SmDevices input_devices,
                       SmSurface input_surface,
                       SmCommandPool* p_command_pool) {
  SmQueueFamilyIndices queueFamilyIndices = find_transfer_queue_families(input_devices.physical_device,
                                                                         input_surface);

  VkCommandPoolCreateInfo commandPoolCreateInfo{};
  commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.graphics_family.value();

  if (VK_SUCCESS != vkCreateCommandPool(input_devices.logical_device,
                                        &commandPoolCreateInfo,
                                        nullptr,
                                        &p_command_pool->command_pool)) {
    throw std::runtime_error("Failed to create command pool");
  }

  std::cout << "Command pool creation process ends with success..." << std::endl;
}

#endif  // SLIMEMAID_SMCOMMANDSSYSTEM_HPP
