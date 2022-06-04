/*
------------------------------------
  Slimemaid Source Code (04.06.2022)
  This file is part of Slimemaid Source Code.
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
