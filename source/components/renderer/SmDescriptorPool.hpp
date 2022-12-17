/*
------------------------------------
  Slimemaid Source Code (04.06.2022)
  This file is part of Slimemaid Source Code.
  Vulkan has use resource descriptors for easily transformation every single frame.
  A descriptor is a way for shaders to freely access resources like buffers and images.
  We're going to set up a buffer that contains the transformation matrices and have the vertex shader access them
  through a descriptor. Usage of descriptors consists of three parts:
    - Specify a descriptor layout during pipeline creation
    - Allocate a descriptor set from a descriptor pool
    - Bind the descriptor set during rendering
  Descriptor sets can't be created directly, they must be allocated from a pool like command buffers.
  The equivalent for descriptor sets is unsurprisingly called a descriptor pool.
  The descriptor pool maintains a pool of descriptors, from which descriptor sets are allocated.
  Descriptor sets are allocated from descriptor pool objects.
------------------------------------
*/

#ifndef SLIMEMAID_SMDESCRIPTORPOOL_HPP
#define SLIMEMAID_SMDESCRIPTORPOOL_HPP

#include <vulkan/vulkan.h>

#include <vector>

struct SmDescriptorPool {
  VkDescriptorPool descriptor_pool;
  std::vector<VkDescriptorSet> descriptor_sets;
  VkDescriptorSetLayout descriptor_set_layout;
};

#endif  // SLIMEMAID_SMDESCRIPTORPOOL_HPP
