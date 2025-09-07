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

#include "stb_image.h"

#include "components/renderer/SmDevices.hpp"
#include "components/renderer/SmTextureImage.hpp"
#include "components/renderer/SmTextureImageViewSampler.hpp"

void sl_generate_mipmaps(uint32_t input_texture_width,
                      uint32_t input_texture_height,
                      uint32_t input_mip_levels,
                      VkImage input_image,
                      VkFormat input_image_format,
                      VkCommandPool& command_pool,
                      VkQueue& graphics_queue,
                      SmDevices& devices);

void sl_create_image(SmDevices input_devices,
                  uint32_t input_width,
                  uint32_t input_height,
                  uint32_t input_mip_levels,
                  VkSampleCountFlagBits input_num_samples,
                  VkFormat input_format,
                  VkImageTiling input_tiling,
                  VkImageUsageFlags input_usage,
                  VkMemoryPropertyFlags input_properties,
                  VkImage& image,
                  VkDeviceMemory& image_memory);

void sl_transition_image_layout(SmDevices& devices,
                             VkCommandPool input_command_pool,
                             VkQueue input_graphics_queue,
                             VkImage input_image,
                             VkFormat input_format,
                             VkImageLayout input_old_image_layout,
                             VkImageLayout input_new_image_layout,
                             uint32_t input_mip_levels);

void sl_create_texture_image(SmDevices input_devices,
                          VkCommandPool& command_pool,
                          VkQueue& graphics_queue,
                          uint32_t& mip_levels,
                          VkImage& texture_image,
                          VkDeviceMemory& texture_image_memory);

void sl_create_texture_sampler(SmDevices input_devices,
                            SmTextureImage input_texture_model_resources,
                            SmTextureImageViewSampler* p_texture_model_resources_read_handler);

#endif  // SLIMEMAID_TEXTUREIMAGE_HPP
