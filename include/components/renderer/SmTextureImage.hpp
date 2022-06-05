/*
------------------------------------
  Slimemaid Source Code (02.06.2022)
  This file is part of Slimemaid Source Code.
  This is main texture image resources of model for Vulkan graphics pipeline.
  Setting up the shader to access the pixel values in the buffer, this is the way to use image objects in Vulkan.
  Texture image is object with model struct and description of using texture type.
  Texture image memory is memory pool of texture data. Texture image should have pointer to this one.
------------------------------------
*/

#ifndef SLIMEMAID_SMTEXTUREIMAGE_HPP
#define SLIMEMAID_SMTEXTUREIMAGE_HPP

#include <vulkan/vulkan.h>

#include <cstdint>  // Necessary for uint32_t

struct SmTextureImage {
  uint32_t mip_levels;
  VkImage texture_image;
  VkDeviceMemory texture_image_memory;
};

#endif  // SLIMEMAID_SMTEXTUREIMAGE_HPP
