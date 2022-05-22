/*
------------------------------------
  Slimemaid Source Code (22.05.2022)
  This file is part of Slimemaid Source Code.
------------------------------------
*/

#ifndef SLIMEMAID_TEXTUREIMAGE_HPP
#define SLIMEMAID_TEXTUREIMAGE_HPP

#define STB_IMAGE_IMPLEMENTATION
#include "SmStencilSystem.hpp"
#include "stb_image.h"
#include "systems/renderer/SmBuffersSystem.hpp"
#include "systems/renderer/SmCommandsSystem.hpp"
#include "systems/renderer/SmGrahicsMemorySystem.hpp"
#include "systems/renderer/SmStencilSystem.hpp"

void generateMipmaps(VkDevice& device,
                     VkPhysicalDevice& physicalDevice,
                     VkCommandPool& commandPool,
                     VkQueue& graphicsQueue,
                     VkImage inputImage,
                     VkFormat inputImageFormat,
                     uint32_t inputTextureWidth,
                     uint32_t inputTextureHeight,
                     uint32_t inputMipLevels) {
  // Check if  image format supports linear blitting
  VkFormatProperties formatProperties;
  vkGetPhysicalDeviceFormatProperties(physicalDevice, inputImageFormat, &formatProperties);

  if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
    throw std::runtime_error("Texture image format does not support linear blitting");
  }

  VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, commandPool);

  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.image = inputImage;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.layerCount = 1;

  int32_t mipWidth = inputTextureWidth;
  int32_t mipHeight = inputTextureHeight;

  for (uint32_t i = 1; i < inputMipLevels; i++) {
    barrier.subresourceRange.baseMipLevel = i - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr,
                         0, nullptr, 1, &barrier);

    VkImageBlit blit{};
    blit.srcOffsets[0] = {0, 0, 0};
    blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
    blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.srcSubresource.mipLevel = i - 1;
    blit.srcSubresource.baseArrayLayer = 0;
    blit.srcSubresource.layerCount = 1;

    int32_t dstMipWidth = 1;
    if (mipWidth > 1) {
      dstMipWidth = mipWidth / 2;
    }

    int32_t dstMipHeight = 1;
    if (mipHeight > 1) {
      dstMipHeight = mipHeight / 2;
    }

    blit.dstOffsets[0] = {0, 0, 0};
    blit.dstOffsets[1] = {dstMipWidth, dstMipHeight, 1};
    blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.dstSubresource.mipLevel = i;
    blit.dstSubresource.baseArrayLayer = 0;
    blit.dstSubresource.layerCount = 1;

    vkCmdBlitImage(commandBuffer,
                   inputImage,
                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   inputImage,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   1,
                   &blit,
                   VK_FILTER_LINEAR);

    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer,
                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         1,
                         &barrier);

    if (mipHeight > 1) {
      mipWidth /= 2;
    }
    if (mipWidth > 1) {
      mipHeight /= 2;
    }
  }

  barrier.subresourceRange.baseMipLevel = inputMipLevels - 1;
  barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

  vkCmdPipelineBarrier(commandBuffer,
                       VK_PIPELINE_STAGE_TRANSFER_BIT,
                       VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                       0,
                       0,
                       nullptr,
                       0,
                       nullptr,
                       1,
                       &barrier);

  endSingleTimeCommands(device,
                        commandPool,
                        graphicsQueue,
                        commandBuffer);
}

void createImage(uint32_t inputWidth,
                 uint32_t inputHeight,
                 uint32_t inputMipLevels,
                 VkSampleCountFlagBits inputNumSamples,
                 VkFormat inputFormat,
                 VkImageTiling inputTiling,
                 VkImageUsageFlags inputUsage,
                 VkMemoryPropertyFlags inputProperties,
                 VkImage& pImage,
                 VkDeviceMemory& pImageMemory,
                 VkPhysicalDevice& physicalDevice,
                 VkDevice& device) {
  VkImageCreateInfo imageInfo{};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width = inputWidth;
  imageInfo.extent.height = inputHeight;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = inputMipLevels;
  imageInfo.arrayLayers = 1;
  imageInfo.format = inputFormat;
  imageInfo.tiling = inputTiling;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage = inputUsage;
  imageInfo.samples = inputNumSamples;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateImage(device, &imageInfo, nullptr, &pImage) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create image");
  }

  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(device, pImage, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = findMemoryType(physicalDevice,
                                             memRequirements.memoryTypeBits,
                                             inputProperties);

  if (vkAllocateMemory(device, &allocInfo, nullptr, &pImageMemory) != VK_SUCCESS) {
    throw std::runtime_error("Failed to allocate image memory");
  }

  vkBindImageMemory(device, pImage, pImageMemory, 0);
}

void transitionImageLayout(VkDevice inputDevice,
                           VkCommandPool inputCommandPool,
                           VkQueue inputGraphicsQueue,
                           VkImage inputImage,
                           VkFormat inputFormat,
                           VkImageLayout inputOldImageLayout,
                           VkImageLayout inputNewImageLayout,
                           uint32_t inputMipLevels) {
  VkCommandBuffer commandBuffer = beginSingleTimeCommands(inputDevice, inputCommandPool);

  VkImageMemoryBarrier imageMemoryBarrier{};
  imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  imageMemoryBarrier.oldLayout = inputOldImageLayout;
  imageMemoryBarrier.newLayout = inputNewImageLayout;
  imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  imageMemoryBarrier.image = inputImage;
  imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
  imageMemoryBarrier.subresourceRange.levelCount = inputMipLevels;
  imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
  imageMemoryBarrier.subresourceRange.layerCount = 1;
  if (VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL == inputNewImageLayout) {
    imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

    if (hasStencilComponent(inputFormat)) {
      imageMemoryBarrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }
  } else {
    imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  }

  VkPipelineStageFlags sourceStage;
  VkPipelineStageFlags destinationStage;

  if (VK_IMAGE_LAYOUT_UNDEFINED == inputOldImageLayout &&
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL == inputNewImageLayout) {
    imageMemoryBarrier.srcAccessMask = 0;
    imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if (VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL == inputOldImageLayout &&
             VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL == inputNewImageLayout) {
    imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else if (VK_IMAGE_LAYOUT_UNDEFINED == inputOldImageLayout &&
             VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL == inputNewImageLayout) {
    imageMemoryBarrier.srcAccessMask = 0;
    imageMemoryBarrier.dstAccessMask =
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  } else {
    throw std::invalid_argument("Unsupported layout transition");
  }

  vkCmdPipelineBarrier(commandBuffer,
                       sourceStage,
                       destinationStage,
                       0,
                       0,
                       nullptr,
                       0,
                       nullptr,
                       1,
                       &imageMemoryBarrier);

  endSingleTimeCommands(inputDevice, inputCommandPool, inputGraphicsQueue, commandBuffer);
}

void createTextureImage(VkDevice& device,
                        VkPhysicalDevice& physicalDevice,
                        VkCommandPool& commandPool,
                        VkQueue& graphicsQueue,
                        uint32_t &mipLevels,
                        VkImage &textureImage,
                        VkDeviceMemory &textureImageMemory) {
  int textureHeight, textureWidth, textureChannels;

  stbi_uc* pixels = stbi_load(TEXTURE_PATH.c_str(),
                              &textureWidth,
                              &textureHeight,
                              &textureChannels,
                              STBI_rgb_alpha);

  VkDeviceSize imageSize = textureWidth * textureHeight * 4;
  mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(textureWidth, textureHeight)))) + 1;

  if (!pixels) {
    throw std::runtime_error("Failed to load raw texture from '" + TEXTURE_PATH + "'");
  } else {
    std::cout << "Texture image '" + TEXTURE_PATH + "' was imported with success..." << std::endl;
  }

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;

  createBuffer(device,
               physicalDevice,
               imageSize,
               VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
               stagingBuffer,
               stagingBufferMemory);

  void* data;
  vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
  memcpy(data, pixels, static_cast<size_t>(imageSize));
  vkUnmapMemory(device, stagingBufferMemory);

  stbi_image_free(pixels);

  createImage(textureWidth,
              textureHeight,
              mipLevels,
              VK_SAMPLE_COUNT_1_BIT,
              VK_FORMAT_R8G8B8A8_SRGB,
              VK_IMAGE_TILING_OPTIMAL,
              VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
              textureImage,
              textureImageMemory,
              physicalDevice,
              device);

  transitionImageLayout(device,
                        commandPool,
                        graphicsQueue,
                        textureImage,
                        VK_FORMAT_R8G8B8A8_SRGB,
                        VK_IMAGE_LAYOUT_UNDEFINED,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        mipLevels);
  copyBufferToImage(device,
                    commandPool,
                    graphicsQueue,
                    stagingBuffer,
                    textureImage,
                    static_cast<size_t>(textureWidth),
                    static_cast<size_t>(textureHeight));
  // transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps

  vkDestroyBuffer(device, stagingBuffer, nullptr);
  vkFreeMemory(device, stagingBufferMemory, nullptr);

  generateMipmaps(device,
                  physicalDevice,
                  commandPool,
                  graphicsQueue,
                  textureImage,
                  VK_FORMAT_R8G8B8A8_SRGB,
                  textureWidth,
                  textureHeight,
                  mipLevels);

  std::cout << "Texture image transition process ends with success..." << std::endl;
}

#endif  // SLIMEMAID_TEXTUREIMAGE_HPP
