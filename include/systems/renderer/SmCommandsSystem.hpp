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

void create_command_buffers(SmDevices input_devices,
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

void create_sync_objects(SmDevices input_devices,
                         SmCommandPool* p_command_pool,
                         std::vector<VkSemaphore>* p_image_available_semaphores,
                         std::vector<VkSemaphore>* p_render_finished_semaphores,
                         std::vector<VkFence>* p_flight_fences) {
  p_image_available_semaphores->resize(p_command_pool->MAX_FRAMES_IN_FLIGHT);
  p_render_finished_semaphores->resize(p_command_pool->MAX_FRAMES_IN_FLIGHT);
  p_flight_fences->resize(p_command_pool->MAX_FRAMES_IN_FLIGHT);

  VkSemaphoreCreateInfo semaphore_create_info{};
  semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fence_create_info{};
  fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < p_command_pool->MAX_FRAMES_IN_FLIGHT; i++) {
    if (VK_SUCCESS != vkCreateSemaphore(input_devices.logical_device,
                                        &semaphore_create_info,
                                        nullptr,
                                        &(*p_image_available_semaphores)[i]) ||
        VK_SUCCESS != vkCreateSemaphore(input_devices.logical_device,
                                        &semaphore_create_info,
                                        nullptr,
                                        &(*p_render_finished_semaphores)[i]) ||
        VK_SUCCESS != vkCreateFence(input_devices.logical_device,
                                    &fence_create_info,
                                    nullptr,
                                    &(*p_flight_fences)[i])) {
      throw std::runtime_error("Failed to create semaphores or fence");
    }
  }

  std::cout << "Sync objects creation process ends with success..." << std::endl;
}

void record_command_buffer(uint32_t image_index,
                           uint32_t input_current_frame_index,
                           SmCommandPool input_command_pool,
                           SmGraphicsPipeline input_graphics_pipeline,
                           SmSwapChain* p_swap_chain,
                           SmDescriptorPool* p_descriptor_pool,
                           SmModelResources* p_model_resources) {
  VkCommandBufferBeginInfo buffer_begin_info{};
  buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  buffer_begin_info.flags = 0;                   // Optional
  buffer_begin_info.pInheritanceInfo = nullptr;  // optional

  if (VK_SUCCESS != vkBeginCommandBuffer(input_command_pool.command_buffers[input_current_frame_index],
                                         &buffer_begin_info)) {
    throw std::runtime_error("Failed to begin recording command buffer");
  }

  VkRenderPassBeginInfo render_pass_begin_info{};
  render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_begin_info.renderPass = input_graphics_pipeline.render_pass;
  render_pass_begin_info.framebuffer = p_swap_chain->swap_chain_frame_buffers[image_index];
  render_pass_begin_info.renderArea.offset = {0, 0};
  render_pass_begin_info.renderArea.extent = p_swap_chain->swap_chain_extent;

  std::array<VkClearValue, 2> clear_values{};
  clear_values[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
  clear_values[1].depthStencil = {1.0f, 0};

  render_pass_begin_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
  render_pass_begin_info.pClearValues = clear_values.data();

  vkCmdBeginRenderPass(input_command_pool.command_buffers[input_current_frame_index],
                       &render_pass_begin_info,
                       VK_SUBPASS_CONTENTS_INLINE);
  vkCmdBindPipeline(input_command_pool.command_buffers[input_current_frame_index],
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    input_graphics_pipeline.pipeline);

  VkBuffer vertexBuffers[] = {p_model_resources->vertex_buffer};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(input_command_pool.command_buffers[input_current_frame_index],
                         0,
                         1,
                         vertexBuffers,
                         offsets);
  vkCmdBindIndexBuffer(input_command_pool.command_buffers[input_current_frame_index],
                       p_model_resources->index_buffer,
                       0,
                       VK_INDEX_TYPE_UINT32);
  vkCmdBindDescriptorSets(input_command_pool.command_buffers[input_current_frame_index],
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          input_graphics_pipeline.pipeline_layout,
                          0,
                          1,
                          p_descriptor_pool->descriptor_sets.data(),
                          0,
                          nullptr);
  vkCmdDrawIndexed(input_command_pool.command_buffers[input_current_frame_index],
                   static_cast<uint32_t>(p_model_resources->indices.size()),
                   1,
                   0,
                   0,
                   0);

  vkCmdEndRenderPass(input_command_pool.command_buffers[input_current_frame_index]);

  if (VK_SUCCESS != vkEndCommandBuffer(input_command_pool.command_buffers[input_current_frame_index])) {
    throw std::runtime_error("Failed to record command buffer");
  }

  // Documentation's description for vkCmdDraw function. Parameters:
  // vertexCount - Even though we don't have a vertex buffer, we technically still have 3
  // scene_model_resources.vertices to draw. instanceCount - Used for instanced rendering, use 1 if you're not doing
  // that. firstVertex - Used as an offset into the vertex buffer, defines the lowest value of gl_VertexIndex.
  // firstInstance - Used as an offset for instanced rendering, defines the lowest value of gl_InstanceIndex.
}

#endif  // SLIMEMAID_SMCOMMANDSSYSTEM_HPP
