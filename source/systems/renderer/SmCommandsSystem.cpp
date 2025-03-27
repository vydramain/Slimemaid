
#include "systems/renderer/SmCommandsSystem.hpp"

#include <iostream>

#include "systems/renderer/SmQueueFamiliesSystem.hpp"

VkCommandBuffer sl_begin_single_time_commands(VkDevice input_device,
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

void sl_end_single_time_commands(VkDevice input_device,
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

void sl_create_command_pool(SmDevices input_devices,
                       SmSurface input_surface,
                       SmCommandPool* p_command_pool) {
  SmQueueFamilyIndices queue_family_indices = find_transfer_queue_families(input_devices.physical_device,
                                                                           input_surface);

  VkCommandPoolCreateInfo command_pool_create_info{};
  command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  command_pool_create_info.queueFamilyIndex = queue_family_indices.graphics_family.value();

  if (VK_SUCCESS != vkCreateCommandPool(input_devices.logical_device,
                                        &command_pool_create_info,
                                        nullptr,
                                        &p_command_pool->command_pool)) {
    throw std::runtime_error("Failed to create command pool");
  }

  std::cout << "Command pool creation process ends with success..." << std::endl;
}

void sl_create_command_buffers(SmDevices input_devices,
                          SmCommandPool* p_command_pool) {
  p_command_pool->command_buffers.resize(p_command_pool->MAX_FRAMES_IN_FLIGHT);

  VkCommandBufferAllocateInfo command_buffer_allocate_info{};
  command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  command_buffer_allocate_info.commandPool = p_command_pool->command_pool;
  command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  command_buffer_allocate_info.commandBufferCount = (uint32_t) p_command_pool->command_buffers.size();

  if (VK_SUCCESS != vkAllocateCommandBuffers(input_devices.logical_device,
                                             &command_buffer_allocate_info,
                                             p_command_pool->command_buffers.data())) {
    throw std::runtime_error("Failed to allocate command buffers");
  }

  std::cout << "Command buffers creation process ends with success..." << std::endl;
}

void sl_create_sync_objects(SmDevices input_devices,
                       SmCommandPool* p_command_pool,
                       std::vector<VkSemaphore>* p_image_available_semaphores,
                       std::vector<VkSemaphore>* p_render_finished_semaphores,
                       std::vector<VkFence>* p_flight_fences) {
  p_image_available_semaphores->resize(p_command_pool->MAX_FRAMES_IN_FLIGHT);
  p_render_finished_semaphores->resize(p_command_pool->MAX_FRAMES_IN_FLIGHT);
  p_flight_fences->resize(p_command_pool->MAX_FRAMES_IN_FLIGHT);

  VkSemaphoreCreateInfo semaphoreCreateInfo{};
  semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceCreateInfo{};
  fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < p_command_pool->MAX_FRAMES_IN_FLIGHT; i++) {
    if (VK_SUCCESS != vkCreateSemaphore(input_devices.logical_device,
                                        &semaphoreCreateInfo,
                                        nullptr,
                                        &(*p_image_available_semaphores)[i]) ||
        VK_SUCCESS != vkCreateSemaphore(input_devices.logical_device,
                                        &semaphoreCreateInfo,
                                        nullptr,
                                        &(*p_render_finished_semaphores)[i]) ||
        VK_SUCCESS != vkCreateFence(input_devices.logical_device,
                                    &fenceCreateInfo,
                                    nullptr,
                                    &(*p_flight_fences)[i])) {
      throw std::runtime_error("Failed to create semaphores or fence");
    }
  }

  std::cout << "Sync objects creation process ends with success..." << std::endl;
}
