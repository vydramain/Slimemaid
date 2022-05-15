/*
------------------------------------

Slimemaid Source Code (08.05.2022)

This file is part of Slimemaid Source Code.

This file has heading of allocation functions for Vulkan API.

------------------------------------
*/

#include <vulkan/vulkan.h>

#include <cstdint> // Necessary for uint32_t

#include "memory_handler.hpp"

/*
------------
Create image function.
This function declare object in graphics device and allocating memory for it.
------------
*/
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
                 VkDevice& inputDevice);
