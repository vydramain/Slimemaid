/*
------------------------------------
  Slimemaid Source Code (22.05.2022)
  This file is part of Slimemaid Source Code.
------------------------------------
*/

#ifndef SLIMEMAID_MODELLOADER_HPP
#define SLIMEMAID_MODELLOADER_HPP

#include <vector>

#include "tiny_obj_loader.h"

const std::string MODEL_PATH = "raws/viking_room/viking_room.obj";
const std::string TEXTURE_PATH = "raws/viking_room/viking_room.png";

void loadModel(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices) {
  tinyobj::attrib_t attribute;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;

  if (!tinyobj::LoadObj(&attribute, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) {
    throw std::runtime_error("Failed to load model from '" + MODEL_PATH + "' because: " + warn + err);
  } else {
    std::cout << "Model from '" + MODEL_PATH + "' was loaded with success..." << std::endl;
  }

  std::unordered_map<Vertex, uint32_t> uniqueVertices{};

  for (const auto& shape : shapes) {
    for (const auto& index : shape.mesh.indices) {
      Vertex vertex{};

      vertex.pos = {attribute.vertices[3 * index.vertex_index + 0], attribute.vertices[3 * index.vertex_index + 1],
                    attribute.vertices[3 * index.vertex_index + 2]};

      vertex.texCoord = {attribute.texcoords[2 * index.texcoord_index + 0],
                         1.0f - attribute.texcoords[2 * index.texcoord_index + 1]};

      vertex.color = {1.0f, 1.0f, 1.0f};

      if (uniqueVertices.count(vertex) == 0) {
        uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
        vertices.push_back(vertex);
      }

      indices.push_back(uniqueVertices[vertex]);
    }
  }
}

#endif  // SLIMEMAID_MODELLOADER_HPP
