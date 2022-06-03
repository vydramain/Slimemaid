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

#include <vector>
#include <cstdint>  // Necessary for uint32_t

#include "components/renderer/Vertex.hpp"

struct SmModelResources {
  std::vector<Vertex> vertices;  // {{rel_x, rel_y, rel_z}, {R, G, B}, {tex_rel_x, tex_rel_x}}
  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;

  std::vector<uint32_t> indices;
  VkBuffer indexBuffer;
  VkDeviceMemory indexBufferMemory;
};

#endif  // SLIMEMAID_SMMODELRESOURCES_HPP
