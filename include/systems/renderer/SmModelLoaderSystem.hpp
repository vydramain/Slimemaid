/*
------------------------------------
  Slimemaid Source Code (22.05.2022)
  This file is part of Slimemaid Source Code.
  Model loader system provide function for load models to special model resources object.
------------------------------------
*/

#ifndef SLIMEMAID_MODELLOADER_HPP
#define SLIMEMAID_MODELLOADER_HPP

#include <vector>

#include "tiny_obj_loader.h"

const std::string MODEL_PATH = "raws/viking_room/viking_room.obj";
const std::string TEXTURE_PATH = "raws/viking_room/viking_room.png";

void loadModel(SmModelResources* p_model_resources) {
  tinyobj::attrib_t attribute;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;

  if (!tinyobj::LoadObj(&attribute, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) {
    throw std::runtime_error("Failed to load model from '" + MODEL_PATH + "' because: " + warn + err);
  } else {
    std::cout << "Model from '" + MODEL_PATH + "' was loaded with success..." << std::endl;
  }

  std::unordered_map<SmVertex, uint32_t> unique_vertices{};

  for (const auto& shape : shapes) {
    for (const auto& index : shape.mesh.indices) {
      SmVertex vertex{};

      vertex.pos = {attribute.vertices[3 * index.vertex_index + 0],
                    attribute.vertices[3 * index.vertex_index + 1],
                    attribute.vertices[3 * index.vertex_index + 2]};

      vertex.tex_coord = {attribute.texcoords[2 * index.texcoord_index + 0],
                         1.0f - attribute.texcoords[2 * index.texcoord_index + 1]};

      vertex.color = {1.0f, 1.0f, 1.0f};

      if (unique_vertices.count(vertex) == 0) {
        unique_vertices[vertex] = static_cast<uint32_t>(p_model_resources->vertices.size());
        p_model_resources->vertices.push_back(vertex);
      }

      p_model_resources->indices.push_back(unique_vertices[vertex]);
    }
  }
}

#endif  // SLIMEMAID_MODELLOADER_HPP
