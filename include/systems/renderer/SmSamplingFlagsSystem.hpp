/*
------------------------------------
  Slimemaid Source Code (09.06.2022)
  This file is part of Slimemaid Source Code.
  The undesired effect as "aliasing" is consequence of a limited numbers of pixels that are available for rendering.
  Since there are no displays out there with unlimited resolution, it will be always visible to some extent.
  This sampling system check device for available a number of samples of Multisample anti-aliasing (MSAA) and set it.
------------------------------------
*/

#ifndef SLIMEMAID_SMSAMPLINGFLAGSSYSTEM_HPP
#define SLIMEMAID_SMSAMPLINGFLAGSSYSTEM_HPP

#include <vulkan/vulkan.h>

VkSampleCountFlagBits get_max_usable_sample_count(VkPhysicalDevice input_device);

#endif  // SLIMEMAID_SMSAMPLINGFLAGSSYSTEM_HPP
