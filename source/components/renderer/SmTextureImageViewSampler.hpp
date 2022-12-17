/*
------------------------------------
  Slimemaid Source Code (03.06.2022)
  This file is part of Slimemaid Source Code.
  This is additional texture image resources of model for Vulkan graphics pipeline. Images access gets
  through image views rather than directly. We will also need to create such an image view for the texture image.
  Sampler objects represent the state of an image sampler which is used by the implementation to read image data
  and apply filtering and other transformations for the shader.
------------------------------------
*/

#ifndef SLIMEMAID_SMTEXTUREIMAGEVIEWSAMPLER_HPP
#define SLIMEMAID_SMTEXTUREIMAGEVIEWSAMPLER_HPP

#include <vulkan/vulkan.h>

struct SmTextureImageViewSampler {
  VkImageView texture_image_view;
  VkSampler texture_sampler;
};

#endif  // SLIMEMAID_SMTEXTUREIMAGEVIEWSAMPLER_HPP
