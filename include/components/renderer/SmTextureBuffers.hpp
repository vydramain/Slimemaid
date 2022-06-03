/*
------------------------------------
  Slimemaid Source Code (02.06.2022)
  This file is part of Slimemaid Source Code.
  Setting up the shader to access the pixel values in the buffer, this is the way to use image objects in Vulkan.
  `texture_image` is object with model struct and description of using texture type.
  `texture_image_memory` is memory pool of texture data.
------------------------------------
*/

#ifndef SLIMEMAID_SMTEXTUREBUFFERS_HPP
#define SLIMEMAID_SMTEXTUREBUFFERS_HPP

#include <vulkan/vulkan.h>

#include <cstdint>  // Necessary for uint32_t

struct SmTextureBuffers {
  VkImage texture_image;
  VkDeviceMemory texture_image_memory;
};

#endif  // SLIMEMAID_SMTEXTUREBUFFERS_HPP
