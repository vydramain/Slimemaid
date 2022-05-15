/*
------------------------------------

Slimemaid Source Code (25.04.2022)

This file is part of Slimemaid Source Code.

This file has heading of allocation functions for Vulkan API.

------------------------------------
*/

#include "renderer/allocator.hpp"

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
                 VkPhysicalDevice& inputPhysicalDevice,
                 VkDevice& inputDevice) {

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

    if (vkCreateImage(inputDevice, &imageInfo, nullptr, &pImage) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create image");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(inputDevice, pImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(inputPhysicalDevice, memRequirements.memoryTypeBits, inputProperties);

    if (vkAllocateMemory(inputDevice, &allocInfo, nullptr, &pImageMemory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate image memory");
    }

    vkBindImageMemory(inputDevice, pImage, pImageMemory, 0);
}
