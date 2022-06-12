/*
------------------------------------
  Slimemaid Source Code (03.06.2022)
  This file is part of Slimemaid Source Code.
------------------------------------
*/

#ifndef SLIMEMAID_SMIMAGEVIEWSYSTEM_HPP
#define SLIMEMAID_SMIMAGEVIEWSYSTEM_HPP

#include <vulkan/vulkan.h>

#include <cstdint>  // Necessary for uint32_t
#include <iostream>

#include "components/renderer/SmDevices.hpp"
#include "components/renderer/SmTextureImage.hpp"
#include "components/renderer/SmTextureImageViewSampler.hpp"

VkImageView create_image_view(SmDevices devices,
                            VkImage inputImage,
                            VkFormat inputFormat,
                            VkImageAspectFlags inputAspectMask,
                            uint32_t inputMipLevels) {
  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = inputImage;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = inputFormat;
  viewInfo.subresourceRange.aspectMask = inputAspectMask;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = inputMipLevels;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  VkImageView imageView;
  if (vkCreateImageView(devices.logical_device,
                        &viewInfo,
                        nullptr,
                        &imageView) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create texture image view");
  }

  return imageView;
}

void createTextureImageView(SmDevices devices,
                            SmTextureImage input_texture_image,
                            SmTextureImageViewSampler& texture_image_view_sampler,
                            uint32_t input_mip_levels) {
  texture_image_view_sampler.texture_image_view = create_image_view(
      devices, input_texture_image.texture_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, input_mip_levels);

  std::cout << "Filling texture image view process ends with success..." << std::endl;
}

#endif  // SLIMEMAID_SMIMAGEVIEWSYSTEM_HPP
