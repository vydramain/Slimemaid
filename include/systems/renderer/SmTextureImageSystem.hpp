/*
------------------------------------
  Slimemaid Source Code (22.05.2022)
  This file is part of Slimemaid Source Code.
  Texture image system create images that represented by multidimensional - up to 3 - arrays of data
  which can be used for textures, by binding them to a graphics or compute pipeline via descriptor sets,
  or by directly specifying them as parameters to certain commands. This data structure is VkImage
  where loaded texture is stored after processing
------------------------------------
*/

#ifndef SLIMEMAID_TEXTUREIMAGE_HPP
#define SLIMEMAID_TEXTUREIMAGE_HPP

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "components/renderer/SmDevices.hpp"

#include "systems/renderer/SmStencilSystem.hpp"
#include "systems/renderer/SmBuffersSystem.hpp"
#include "systems/renderer/SmCommandsSystem.hpp"
#include "systems/renderer/SmGrahicsMemorySystem.hpp"
#include "systems/renderer/SmStencilSystem.hpp"

void generate_mipmaps(uint32_t input_texture_width,
                      uint32_t input_texture_height,
                      uint32_t input_mip_levels,
                      VkImage input_image,
                      VkFormat input_image_format,
                      VkCommandPool& command_pool,
                      VkQueue& graphics_queue,
                      SmDevices& devices) {
  VkFormatProperties format_properties; // Check if image format supports linear blitting
  vkGetPhysicalDeviceFormatProperties(devices.physical_device,
                                      input_image_format,
                                      &format_properties);

  if (!(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
    throw std::runtime_error("Texture image format does not support linear blitting");
  }

  VkCommandBuffer command_buffer = begin_single_time_commands(devices.logical_device, command_pool);

  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.image = input_image;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.layerCount = 1;

  int32_t mip_width = input_texture_width;
  int32_t mip_height = input_texture_height;

  for (uint32_t i = 1; i < input_mip_levels; i++) {
    barrier.subresourceRange.baseMipLevel = i - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

    vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr,
                         0, nullptr, 1, &barrier);

    VkImageBlit blit{};
    blit.srcOffsets[0] = {0, 0, 0};
    blit.srcOffsets[1] = {mip_width, mip_height, 1};
    blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.srcSubresource.mipLevel = i - 1;
    blit.srcSubresource.baseArrayLayer = 0;
    blit.srcSubresource.layerCount = 1;

    int32_t dst_mip_width = 1;
    if (mip_width > 1) {
      dst_mip_width = mip_width / 2;
    }

    int32_t dst_mip_height = 1;
    if (mip_height > 1) {
      dst_mip_height = mip_height / 2;
    }

    blit.dstOffsets[0] = {0, 0, 0};
    blit.dstOffsets[1] = {dst_mip_width, dst_mip_height, 1};
    blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.dstSubresource.mipLevel = i;
    blit.dstSubresource.baseArrayLayer = 0;
    blit.dstSubresource.layerCount = 1;

    vkCmdBlitImage(command_buffer, input_image,
                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, input_image,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   1,
                   &blit,
                   VK_FILTER_LINEAR);

    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(command_buffer,
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         1,
                         &barrier);

    if (mip_height > 1) {
      mip_width /= 2;
    }
    if (mip_width > 1) {
      mip_height /= 2;
    }
  }

  barrier.subresourceRange.baseMipLevel = input_mip_levels - 1;
  barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

  vkCmdPipelineBarrier(command_buffer,
                       VK_PIPELINE_STAGE_TRANSFER_BIT,
                       VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                       0,
                       0,
                       nullptr,
                       0,
                       nullptr,
                       1,
                       &barrier);

  end_single_time_commands(devices.logical_device, command_pool, graphics_queue, command_buffer);
}

void create_image(SmDevices input_devices,
                  uint32_t input_width,
                  uint32_t input_height,
                  uint32_t input_mip_levels,
                  VkSampleCountFlagBits input_num_samples,
                  VkFormat input_format,
                  VkImageTiling input_tiling,
                  VkImageUsageFlags input_usage,
                  VkMemoryPropertyFlags input_properties,
                  VkImage& image,
                  VkDeviceMemory& image_memory) {
  VkImageCreateInfo image_info{};
  image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_info.imageType = VK_IMAGE_TYPE_2D;
  image_info.extent.width = input_width;
  image_info.extent.height = input_height;
  image_info.extent.depth = 1;
  image_info.mipLevels = input_mip_levels;
  image_info.arrayLayers = 1;
  image_info.format = input_format;
  image_info.tiling = input_tiling;
  image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  image_info.usage = input_usage;
  image_info.samples = input_num_samples;
  image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateImage(input_devices.logical_device, &image_info, nullptr, &image) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create image");
  }

  VkMemoryRequirements mem_requirements;
  vkGetImageMemoryRequirements(input_devices.logical_device, image, &mem_requirements);

  VkMemoryAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = mem_requirements.size;
  alloc_info.memoryTypeIndex =
      find_memory_type(input_devices,
                       mem_requirements.memoryTypeBits,
                       input_properties);

  if (vkAllocateMemory(input_devices.logical_device, &alloc_info, nullptr, &image_memory) != VK_SUCCESS) {
    throw std::runtime_error("Failed to allocate image memory");
  }

  vkBindImageMemory(input_devices.logical_device, image, image_memory, 0);
}

void transition_image_layout(SmDevices& devices,
                             VkCommandPool input_command_pool,
                             VkQueue input_graphics_queue,
                             VkImage input_image,
                             VkFormat input_format,
                             VkImageLayout input_old_image_layout,
                             VkImageLayout input_new_image_layout,
                             uint32_t input_mip_levels) {
  VkCommandBuffer command_buffer = begin_single_time_commands(devices.logical_device, input_command_pool);

  VkImageMemoryBarrier image_memory_barrier{};
  image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  image_memory_barrier.oldLayout = input_old_image_layout;
  image_memory_barrier.newLayout = input_new_image_layout;
  image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  image_memory_barrier.image = input_image;
  image_memory_barrier.subresourceRange.baseMipLevel = 0;
  image_memory_barrier.subresourceRange.levelCount = input_mip_levels;
  image_memory_barrier.subresourceRange.baseArrayLayer = 0;
  image_memory_barrier.subresourceRange.layerCount = 1;
  if (VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL == input_new_image_layout) {
    image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

    if (hasStencilComponent(input_format)) {
      image_memory_barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }
  } else {
    image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  }

  VkPipelineStageFlags src_stage;
  VkPipelineStageFlags dst_stage;

  if (VK_IMAGE_LAYOUT_UNDEFINED == input_old_image_layout &&
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL == input_new_image_layout) {
    image_memory_barrier.srcAccessMask = 0;
    image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if (VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL == input_old_image_layout &&
             VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL == input_new_image_layout) {
    image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else if (VK_IMAGE_LAYOUT_UNDEFINED == input_old_image_layout &&
             VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL == input_new_image_layout) {
    image_memory_barrier.srcAccessMask = 0;
    image_memory_barrier.dstAccessMask =
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    dst_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  } else {
    throw std::invalid_argument("Unsupported layout transition");
  }

  vkCmdPipelineBarrier(command_buffer,
                       src_stage,
                       dst_stage,
                       0,
                       0,
                       nullptr,
                       0,
                       nullptr,
                       1,
                       &image_memory_barrier);

  end_single_time_commands(devices.logical_device, input_command_pool, input_graphics_queue, command_buffer);
}

void create_texture_image(SmDevices input_devices,
                          VkCommandPool& command_pool,
                          VkQueue& graphics_queue,
                          uint32_t& mip_levels,
                          VkImage& texture_image,
                          VkDeviceMemory& texture_image_memory) {
  int texture_height, texture_width, texture_channels;

  stbi_uc* pixels = stbi_load(TEXTURE_PATH.c_str(),
                              &texture_width,
                              &texture_height,
                              &texture_channels,
                              STBI_rgb_alpha);

  VkDeviceSize image_size = texture_width * texture_height * 4; // 4 is size for layers: RGBA
  mip_levels = static_cast<uint32_t>(std::floor(std::log2(std::max(texture_width, texture_height)))) + 1;

  if (!pixels) {
    throw std::runtime_error("Failed to load raw texture from '" + TEXTURE_PATH + "'");
  } else {
    std::cout << "Texture image '" + TEXTURE_PATH + "' was imported with success..." << std::endl;
  }

  VkBuffer staging_buffer;
  VkDeviceMemory staging_buffer_memory;

  create_buffer(input_devices,
                image_size,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                &staging_buffer,
                &staging_buffer_memory);

  void* data;
  vkMapMemory(input_devices.logical_device,
              staging_buffer_memory,
              0,
              image_size,
              0,
              &data);
  memcpy(data, pixels, static_cast<size_t>(image_size));
  vkUnmapMemory(input_devices.logical_device, staging_buffer_memory);

  stbi_image_free(pixels);

  create_image(input_devices,
               texture_width,
               texture_height, mip_levels,
               VK_SAMPLE_COUNT_1_BIT,
               VK_FORMAT_R8G8B8A8_SRGB,
               VK_IMAGE_TILING_OPTIMAL,
               VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
               texture_image,
               texture_image_memory);

  transition_image_layout(input_devices, command_pool, graphics_queue,
                          texture_image,
                          VK_FORMAT_R8G8B8A8_SRGB,
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mip_levels);
  copy_buffer_to_image(input_devices, command_pool, graphics_queue,
                       staging_buffer,
                       texture_image,
                       static_cast<size_t>(texture_width),
                       static_cast<size_t>(texture_height));
  // transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps

  vkDestroyBuffer(input_devices.logical_device, staging_buffer, nullptr);
  vkFreeMemory(input_devices.logical_device, staging_buffer_memory, nullptr);

  generate_mipmaps(texture_width,
                   texture_height, mip_levels,
                   texture_image,
                   VK_FORMAT_R8G8B8A8_SRGB, command_pool,
                   graphics_queue, input_devices);

  std::cout << "Texture image transition process ends with success..." << std::endl;
}

void create_texture_sampler(SmDevices input_devices,
                            SmTextureImage input_texture_model_resources,
                            SmTextureImageViewSampler* p_texture_model_resources_read_handler) {
  VkPhysicalDeviceProperties device_properties{};
  vkGetPhysicalDeviceProperties(input_devices.physical_device, &device_properties);

  VkSamplerCreateInfo sampler_create_info{};
  sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  sampler_create_info.magFilter = VK_FILTER_LINEAR;
  sampler_create_info.minFilter = VK_FILTER_LINEAR;
  sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  sampler_create_info.anisotropyEnable = VK_TRUE;
  sampler_create_info.maxAnisotropy = device_properties.limits.maxSamplerAnisotropy;
  sampler_create_info.borderColor = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
  sampler_create_info.unnormalizedCoordinates = VK_FALSE;
  sampler_create_info.compareEnable = VK_FALSE;
  sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
  sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  sampler_create_info.mipLodBias = 0.0f;
  sampler_create_info.minLod = 0.0f;
  sampler_create_info.maxLod = static_cast<float>(input_texture_model_resources.mip_levels);

  if (VK_SUCCESS != vkCreateSampler(input_devices.logical_device,
                                    &sampler_create_info,
                                    nullptr,
                                    &p_texture_model_resources_read_handler->texture_sampler)) {
    throw std::runtime_error("Failed to create texture sampler");
  }

  std::cout << "Texture sampler creation process ends with success..." << std::endl;
}

#endif  // SLIMEMAID_TEXTUREIMAGE_HPP
