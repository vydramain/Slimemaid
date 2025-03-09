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

#include "components/renderer/SmDevices.hpp"
#include "components/renderer/SmTextureImage.hpp"
#include "components/renderer/SmTextureImageViewSampler.hpp"

VkImageView create_image_view(SmDevices devices,
                            VkImage input_image,
                            VkFormat input_format,
                            VkImageAspectFlags input_aspect_mask,
                            uint32_t input_mip_levels);


void createTextureImageView(SmDevices devices,
                            SmTextureImage input_texture_image,
                            SmTextureImageViewSampler& texture_image_view_sampler,
                            uint32_t input_mip_levels);

#endif  // SLIMEMAID_SMIMAGEVIEWSYSTEM_HPP
