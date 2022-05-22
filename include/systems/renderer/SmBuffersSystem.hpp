/*
------------------------------------
  Slimemaid Source Code (22.05.2022)
  This file is part of Slimemaid Source Code.
------------------------------------
*/

#ifndef SLIMEMAID_SMIMAGECREATIONSYSTEM_HPP
#define SLIMEMAID_SMIMAGECREATIONSYSTEM_HPP

#include <vulkan/vulkan.h>

#include <cstdio>
#include <stdexcept>

#include "systems/renderer/SmCommandsSystem.hpp"
#include "systems/renderer/SmGrahicsMemorySystem.hpp"

void createBuffer(VkDevice& device,
                  VkPhysicalDevice& physicalDevice,
                  VkDeviceSize inputSize,
                  VkBufferUsageFlags inputUsage,
                  VkMemoryPropertyFlags inputMemoryProperties,
                  VkBuffer& buffer,
                  VkDeviceMemory& bufferMemory) {
  VkBufferCreateInfo bufferCreateInfo{};
  bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferCreateInfo.size = inputSize;
  bufferCreateInfo.usage = inputUsage;
  bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (VK_SUCCESS != vkCreateBuffer(device, &bufferCreateInfo, nullptr, &buffer)) {
    throw std::runtime_error("Failed to create buffer");
  }

  VkMemoryRequirements bufferMemRequirements;
  vkGetBufferMemoryRequirements(device, buffer, &bufferMemRequirements);

  VkMemoryAllocateInfo memAllocateInfo{};
  memAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  memAllocateInfo.allocationSize = bufferMemRequirements.size;
  memAllocateInfo.memoryTypeIndex = findMemoryType(physicalDevice,
                                                   bufferMemRequirements.memoryTypeBits,
                                                   inputMemoryProperties);

  if (VK_SUCCESS != vkAllocateMemory(device, &memAllocateInfo, nullptr, &bufferMemory)) {
    throw std::runtime_error("Failed to allocate buffer memory");
  }

  vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void copyBufferToImage(VkDevice inputDevice,
                       VkCommandPool inputCommandPool,
                       VkQueue inputGraphicsQueue,
                       VkBuffer inputBuffer,
                       VkImage inputImage,
                       uint32_t inputWidth,
                       uint32_t inputHeight) {
  VkCommandBuffer commandBuffer = beginSingleTimeCommands(inputDevice, inputCommandPool);

  VkBufferImageCopy imageRegion{};
  imageRegion.bufferOffset = 0;
  imageRegion.bufferRowLength = 0;
  imageRegion.bufferImageHeight = 0;
  imageRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  imageRegion.imageSubresource.mipLevel = 0;
  imageRegion.imageSubresource.baseArrayLayer = 0;
  imageRegion.imageSubresource.layerCount = 1;
  imageRegion.imageOffset = {0, 0, 0};
  imageRegion.imageExtent = {inputWidth, inputHeight, 1};

  vkCmdCopyBufferToImage(commandBuffer, inputBuffer, inputImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                         &imageRegion);

  endSingleTimeCommands(inputDevice, inputCommandPool, inputGraphicsQueue, commandBuffer);
}

#endif  // SLIMEMAID_SMIMAGECREATIONSYSTEM_HPP
