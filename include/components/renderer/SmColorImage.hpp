/*
------------------------------------
  Slimemaid Source Code (04.06.2022)
  This file is part of Slimemaid Source Code.
  Component keeps color info for texture or some polygon filling:
    - Images represent multidimensional - up to 3 - arrays of data which can be used for various purposes
      (e.g. attachments, textures), by binding them to a graphics or compute pipeline via descriptor sets.
    - Image objects are not directly accessed by pipeline shaders for reading or writing image data.
      Instead, image views representing contiguous ranges of the image subresources and containing additional metadata
      are used for that purpose. Views must be created on images of compatible types,
      and must represent a valid subset of image subresources.
------------------------------------
*/

#ifndef SLIMEMAID_SMCOLORIMAGE_HPP
#define SLIMEMAID_SMCOLORIMAGE_HPP

#include <vulkan/vulkan.h>

struct SmColorImage {
  VkImage colorImage;
  VkDeviceMemory colorImageMemory;
  VkImageView colorImageView;
};

#endif  // SLIMEMAID_SMCOLORIMAGE_HPP
