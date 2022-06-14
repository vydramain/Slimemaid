/*
------------------------------------
  Slimemaid Source Code (03.06.2022)
  This file is part of Slimemaid Source Code.

------------------------------------
*/

#ifndef SLIMEMAID_SMIMAGEVIEWSYSTEM_HPP
#define SLIMEMAID_SMIMAGEVIEWSYSTEM_HPP

#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>

#include "components/renderer/SmDevices.hpp"
#include "components/renderer/SmTextureImage.hpp"
#include "components/renderer/SmTextureImageViewSampler.hpp"

VkImageView create_image_view(SmDevices devices,
                              VkImage input_image,
                              VkFormat input_format,
                              VkImageAspectFlags input_aspect_mask,
                              uint32_t input_mip_levels) {
  VkImageViewCreateInfo view_info{};
  view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  view_info.image = input_image;
  view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  view_info.format = input_format;
  view_info.subresourceRange.aspectMask = input_aspect_mask;
  view_info.subresourceRange.baseMipLevel = 0;
  view_info.subresourceRange.levelCount = input_mip_levels;
  view_info.subresourceRange.baseArrayLayer = 0;
  view_info.subresourceRange.layerCount = 1;

  VkImageView image_view;
  if (vkCreateImageView(devices.logical_device,
                        &view_info,
                        nullptr,
                        &image_view) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create texture image view");
  }

  return image_view;
}

void create_texture_image_view(SmDevices input_devices,
                               SmTextureImage input_texture_image,
                               SmTextureImageViewSampler* p_texture_image_view_sampler,
                               uint32_t input_mip_levels) {
  p_texture_image_view_sampler->texture_image_view = create_image_view(input_devices,
                                                                       input_texture_image.texture_image,
                                                                       VK_FORMAT_R8G8B8A8_SRGB,
                                                                       VK_IMAGE_ASPECT_COLOR_BIT,
                                                                       input_mip_levels);

  std::cout << "Filling texture image view process ends with success..." << std::endl;
}

#endif  // SLIMEMAID_SMIMAGEVIEWSYSTEM_HPP
