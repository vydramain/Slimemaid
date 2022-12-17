/*
------------------------------------
  Slimemaid Source Code (04.06.2022)
  This file is part of Slimemaid Source Code.
  Uniform buffer contains the UBO data for the shaders.
  Multiple buffers, because multiple frames may be in flight at the same time we don't need to update the buffer
  in preparation of the next frame while a previous one is still reading from it.
------------------------------------
*/

#ifndef SLIMEMAID_SMUNIFORMBUFFERS_HPP
#define SLIMEMAID_SMUNIFORMBUFFERS_HPP

#include <vulkan/vulkan.h>

#include <vector>

struct SmUniformBuffers {
  std::vector<VkBuffer> uniform_buffers;
  std::vector<VkDeviceMemory> uniform_buffers_memory;
};

#endif  // SLIMEMAID_SMUNIFORMBUFFERS_HPP
