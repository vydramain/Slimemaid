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

#include <cstdio>
#include <stdexcept>

#include "components/renderer/SmDevices.hpp"

#include "systems/renderer/SmCommandsSystem.hpp"
#include "systems/renderer/SmGrahicsMemorySystem.hpp"

void create_buffer(SmDevices* p_devices,
                   VkDeviceSize input_size,
                   VkBufferUsageFlags input_usage,
                   VkMemoryPropertyFlags input_memory_properties,
                   VkBuffer* p_buffer,
                   VkDeviceMemory* p_buffer_memory) {
  VkBufferCreateInfo buffer_create_info{};
  buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_create_info.size = input_size;
  buffer_create_info.usage = input_usage;
  buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (VK_SUCCESS != vkCreateBuffer(p_devices->logical_device,
                                   &buffer_create_info,
                                   nullptr,
                                   p_buffer)) {
    throw std::runtime_error("Failed to create p_buffer");
  }

  VkMemoryRequirements buffer_mem_requirements;
  vkGetBufferMemoryRequirements(p_devices->logical_device,
                                *p_buffer,
                                &buffer_mem_requirements);

  VkMemoryAllocateInfo mem_allocate_info{};
  mem_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  mem_allocate_info.allocationSize = buffer_mem_requirements.size;
  mem_allocate_info.memoryTypeIndex = find_memory_type(p_devices,
                                                       buffer_mem_requirements.memoryTypeBits,
                                                       input_memory_properties);

  if (VK_SUCCESS != vkAllocateMemory(p_devices->logical_device,
                                     &mem_allocate_info,
                                     nullptr,
                                     p_buffer_memory)) {
    throw std::runtime_error("Failed to allocate p_buffer memory");
  }

  vkBindBufferMemory(p_devices->logical_device,
                     *p_buffer,
                     *p_buffer_memory,
                     0);
}

void copy_buffer(SmDevices* p_devices,
                SmCommandPool* p_command_pool,
                SmQueues* p_queues,
                VkBuffer input_src_buffer,
                VkBuffer input_dst_buffer,
                VkDeviceSize input_buffer_size) {
  VkCommandBuffer command_buffer = beginSingleTimeCommands(p_devices->logical_device,
                                                           p_command_pool->command_pool);

  VkBufferCopy copy_region{};
  copy_region.srcOffset = 0;  // Optional
  copy_region.dstOffset = 0;  // Optional
  copy_region.size = input_buffer_size;
  vkCmdCopyBuffer(command_buffer,
                  input_src_buffer,
                  input_dst_buffer,
                  1,
                  &copy_region);

  endSingleTimeCommands(p_devices->logical_device,
                        p_command_pool->command_pool,
                        p_queues->graphics_queue,
                        command_buffer);
}

void copy_buffer_to_image(SmDevices input_devices,
                          VkCommandPool input_command_pool,
                          VkQueue input_graphics_queue,
                          VkBuffer input_buffer,
                          VkImage input_image,
                          uint32_t input_width,
                          uint32_t input_height) {
  VkCommandBuffer command_buffer = beginSingleTimeCommands(input_devices.logical_device,
                                                           input_command_pool);

  VkBufferImageCopy image_region{};
  image_region.bufferOffset = 0;
  image_region.bufferRowLength = 0;
  image_region.bufferImageHeight = 0;
  image_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  image_region.imageSubresource.mipLevel = 0;
  image_region.imageSubresource.baseArrayLayer = 0;
  image_region.imageSubresource.layerCount = 1;
  image_region.imageOffset = {0, 0, 0};
  image_region.imageExtent = {input_width, input_height, 1};

  vkCmdCopyBufferToImage(command_buffer,
                         input_buffer,
                         input_image,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                         1,
                         &image_region);

  endSingleTimeCommands(input_devices.logical_device,
                        input_command_pool,
                        input_graphics_queue,
                        command_buffer);
}

void create_vertex_buffer(SmDevices* p_devices,
                          SmCommandPool* p_command_pool,
                          SmQueues* p_queues,
                          SmModelResources* p_model_resources) {
  VkDeviceSize buffer_size = sizeof(p_model_resources->vertices[0]) * p_model_resources->vertices.size();

  VkBuffer staging_buffer;
  VkDeviceMemory staging_buffer_memory;
  create_buffer(p_devices,
                buffer_size,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                &staging_buffer,
                &staging_buffer_memory);

  void* data;
  vkMapMemory(p_devices->logical_device,
              staging_buffer_memory,
              0,
              buffer_size,
              0,
              &data);
  memcpy(data,
         p_model_resources->vertices.data(),
         (size_t) buffer_size);
  vkUnmapMemory(p_devices->logical_device,
                staging_buffer_memory);

  create_buffer(p_devices,
                buffer_size,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                &p_model_resources->vertex_buffer,
                &p_model_resources->vertex_buffer_memory);

  copy_buffer(p_devices,
              p_command_pool,
              p_queues,
              staging_buffer,
              p_model_resources->vertex_buffer,
              buffer_size);

  vkDestroyBuffer(p_devices->logical_device, staging_buffer, nullptr);
  vkFreeMemory(p_devices->logical_device, staging_buffer_memory, nullptr);

  std::cout << "SmVertex buffer creation process ends with success..." << std::endl;
}

void create_index_buffer(SmDevices* p_devices,
                       SmCommandPool* p_command_pool,
                       SmQueues* p_queues,
                       SmModelResources* p_model_resources) {
  VkDeviceSize buffer_size = sizeof(p_model_resources->indices[0]) * p_model_resources->indices.size();

  VkBuffer staging_buffer;
  VkDeviceMemory staging_buffer_memory;
  create_buffer(p_devices,
                buffer_size,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                &staging_buffer,
                &staging_buffer_memory);

  void* data;
  vkMapMemory(p_devices->logical_device,
              staging_buffer_memory,
              0,
              buffer_size,
              0,
              &data);
  memcpy(data,
         p_model_resources->indices.data(),
         (size_t) buffer_size);
  vkUnmapMemory(p_devices->logical_device,
                staging_buffer_memory);

  create_buffer(p_devices,
                buffer_size,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                &p_model_resources->index_buffer,
                &p_model_resources->index_buffer_memory);

  copy_buffer(p_devices,
              p_command_pool,
              p_queues,
              staging_buffer,
              p_model_resources->index_buffer,
              buffer_size);

  vkDestroyBuffer(p_devices->logical_device, staging_buffer, nullptr);
  vkFreeMemory(p_devices->logical_device, staging_buffer_memory, nullptr);

  std::cout << "Index buffer creation process ends with success..." << std::endl;
}

#endif  // SLIMEMAID_SMIMAGECREATIONSYSTEM_HPP
