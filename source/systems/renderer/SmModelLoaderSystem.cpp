/*
------------------------------------
 Slimemaid Source Code (15.06.2022)
 This file is part of Slimemaid Source Code.
----------------------------------
*/

#include "systems/renderer/SmModelLoaderSystem.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

void load_model(SmModelResources* p_model_resources) {
  tinyobj::attrib_t attribute;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;

  if (!tinyobj::LoadObj(&attribute,
                        &shapes,
                        &materials,
                        &warn,
                        &err,
                        MODEL_PATH.c_str())) {
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
