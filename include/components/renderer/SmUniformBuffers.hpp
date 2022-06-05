/*
------------------------------------
  Slimemaid Source Code (04.06.2022)
  This file is part of Slimemaid Source Code.
  Buffers represent linear arrays of data which are used for various purposes by binding them to
  a uniform buffers objects.
------------------------------------
*/

#ifndef SLIMEMAID_SMUNIFORMBUFFERS_HPP
#define SLIMEMAID_SMUNIFORMBUFFERS_HPP

#include <vulkan/vulkan.h>

#include <vector>

struct SmUniformBuffers {
  std::vector<VkBuffer> uniformBuffers;
  std::vector<VkDeviceMemory> uniformBuffersMemory;
};

#endif  // SLIMEMAID_SMUNIFORMBUFFERS_HPP
