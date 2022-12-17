/*
------------------------------------
  Slimemaid Source Code (22.05.2022)
  This file is part of Slimemaid Source Code.
  Buffers system create and copy any buffers. Buffers creation process include creating describing struct for buffer,
  buffer and allocating host memory (by Vulkan implementations). Coping process use Vulkan tools for buffers copy.

------------------------------------
*/

#ifndef SLIMEMAID_SMIMAGECREATIONSYSTEM_HPP
#define SLIMEMAID_SMIMAGECREATIONSYSTEM_HPP

#include <vulkan/vulkan.h>

#include <cstring>
#include <iostream>
#include <stdexcept>

#include "systems/renderer/SmCommandsSystem.hpp"
#include "systems/renderer/SmGraphicsMemorySystem.hpp"

#include "components/renderer/SmCommandPool.hpp"
#include "components/renderer/SmDevices.hpp"
#include "components/renderer/SmModelResources.hpp"
#include "components/renderer/SmQueues.hpp"
#include "components/renderer/SmUniformBufferObject.hpp"
#include "components/renderer/SmUniformBuffers.hpp"

void create_buffer(SmDevices input_devices,
                   VkDeviceSize input_size,
                   VkBufferUsageFlags input_usage,
                   VkMemoryPropertyFlags input_memory_properties,
                   VkBuffer* p_buffer,
                   VkDeviceMemory* p_buffer_memory);

void copy_buffer(SmDevices input_devices,
                 SmCommandPool* p_command_pool,
                 SmQueues* p_queues,
                 VkBuffer input_src_buffer,
                 VkBuffer input_dst_buffer,
                 VkDeviceSize input_buffer_size);

void copy_buffer_to_image(SmDevices input_devices,
                          VkCommandPool input_command_pool,
                          VkQueue input_graphics_queue,
                          VkBuffer input_buffer,
                          VkImage input_image,
                          uint32_t input_width,
                          uint32_t input_height);

void create_vertex_buffer(SmDevices input_devices,
                          SmCommandPool* p_command_pool,
                          SmQueues* p_queues,
                          SmModelResources* p_model_resources);

void create_index_buffer(SmDevices input_devices,
                       SmCommandPool* p_command_pool,
                       SmQueues* p_queues,
                       SmModelResources* p_model_resources);

void create_uniform_buffers(SmDevices input_devices,
                            SmCommandPool* p_command_pool,
                            SmUniformBuffers* p_uniform_buffers);

#endif  // SLIMEMAID_SMIMAGECREATIONSYSTEM_HPP
