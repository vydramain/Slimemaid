/*
------------------------------------
  Slimemaid Source Code (03.06.2022)
  This file is part of Slimemaid Source Code.
  SmModelResources struct keeps textured 3D meshes resources.
------------------------------------
*/

#ifndef SLIMEMAID_SMMODELRESOURCES_HPP
#define SLIMEMAID_SMMODELRESOURCES_HPP

#include <vulkan/vulkan.h>

#include <cstdint>  // Necessary for uint32_t
#include <vector>

#include "components/renderer/SmVertex.hpp"

struct SmModelResources {
  std::vector<SmVertex> vertices;  // {{rel_x, rel_y, rel_z}, {R, G, B}, {tex_rel_x, tex_rel_x}}
  VkBuffer vertex_buffer;
  VkDeviceMemory vertex_buffer_memory;

  std::vector<uint32_t> indices;
  VkBuffer index_buffer;
  VkDeviceMemory index_buffer_memory;
};

#endif  // SLIMEMAID_SMMODELRESOURCES_HPP
