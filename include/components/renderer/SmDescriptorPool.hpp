/*
------------------------------------
  Slimemaid Source Code (04.06.2022)
  This file is part of Slimemaid Source Code.
  A descriptor pool maintains a pool of descriptors, from which descriptor sets are allocated.
  Descriptor sets are allocated from descriptor pool objects
------------------------------------
*/

#ifndef SLIMEMAID_SMDESCRIPTORPOOL_HPP
#define SLIMEMAID_SMDESCRIPTORPOOL_HPP

#include <vulkan/vulkan.h>

#include <vector>

struct SmDescriptorPool {
  VkDescriptorPool descriptorPool;
  std::vector<VkDescriptorSet> descriptorSets;
};

#endif  // SLIMEMAID_SMDESCRIPTORPOOL_HPP
