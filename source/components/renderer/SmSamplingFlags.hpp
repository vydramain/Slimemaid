/*
------------------------------------
  Slimemaid Source Code (05.06.2022)
  This file is part of Slimemaid Source Code.
  Sampling flag set multisample anti-aliasing parameter.
  By default, using only one sample per pixel which is equivalent to no multisampling.
------------------------------------
*/

#ifndef SLIMEMAID_SMSAMPLINGFLAGS_HPP
#define SLIMEMAID_SMSAMPLINGFLAGS_HPP

#include <vulkan/vulkan.h>

struct SmSamplingFlags {
  VkSampleCountFlagBits msaa_samples = VK_SAMPLE_COUNT_1_BIT;
};

#endif  // SLIMEMAID_SMSAMPLINGFLAGS_HPP
