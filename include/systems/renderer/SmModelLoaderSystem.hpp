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

void loadModel(SmModelResources& model_resources) {
  tinyobj::attrib_t attribute;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;

  if (!tinyobj::LoadObj(&attribute, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) {
    throw std::runtime_error("Failed to load model from '" + MODEL_PATH + "' because: " + warn + err);
  } else {
    std::cout << "Model from '" + MODEL_PATH + "' was loaded with success..." << std::endl;
  }

  std::unordered_map<SmVertex, uint32_t> uniqueVertices{};

  for (const auto& shape : shapes) {
    for (const auto& index : shape.mesh.indices) {
      SmVertex vertex{};

      vertex.pos = {attribute.vertices[3 * index.vertex_index + 0], attribute.vertices[3 * index.vertex_index + 1],
                    attribute.vertices[3 * index.vertex_index + 2]};

      vertex.tex_coord = {attribute.texcoords[2 * index.texcoord_index + 0],
                         1.0f - attribute.texcoords[2 * index.texcoord_index + 1]};

      vertex.color = {1.0f, 1.0f, 1.0f};

      if (uniqueVertices.count(vertex) == 0) {
        uniqueVertices[vertex] = static_cast<uint32_t>(model_resources.vertices.size());
        model_resources.vertices.push_back(vertex);
      }

      model_resources.indices.push_back(uniqueVertices[vertex]);
    }
  }
}

void createVertexBuffer(SmDevices& devices,
                        SmCommandPool& command_pool,
                        SmQueues& queues,
                        SmModelResources& model_resources) {
  VkDeviceSize bufferSize = sizeof(model_resources.vertices[0]) * model_resources.vertices.size();

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  createBuffer(devices.logical_device, devices.physical_device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
               stagingBufferMemory);

  void* data;
  vkMapMemory(devices.logical_device, stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, model_resources.vertices.data(), (size_t) bufferSize);
  vkUnmapMemory(devices.logical_device, stagingBufferMemory);

  createBuffer(devices.logical_device, devices.physical_device, bufferSize,
               VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, model_resources.vertex_buffer,
               model_resources.vertex_buffer_memory);

  copyBuffer(devices,
             command_pool,
             queues,
             stagingBuffer,
             model_resources.vertex_buffer, bufferSize);

  vkDestroyBuffer(devices.logical_device, stagingBuffer, nullptr);
  vkFreeMemory(devices.logical_device, stagingBufferMemory, nullptr);

  std::cout << "SmVertex buffer creation process ends with success..." << std::endl;
}

void createIndexBuffer(SmDevices& devices,
                       SmCommandPool& command_pool,
                       SmQueues& queues,
                       SmModelResources& model_resources) {
  VkDeviceSize bufferSize = sizeof(model_resources.indices[0]) * model_resources.indices.size();

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  createBuffer(devices.logical_device, devices.physical_device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
               stagingBufferMemory);

  void* data;
  vkMapMemory(devices.logical_device, stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, model_resources.indices.data(), (size_t) bufferSize);
  vkUnmapMemory(devices.logical_device, stagingBufferMemory);

  createBuffer(devices.logical_device, devices.physical_device, bufferSize,
               VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, model_resources.index_buffer,
               model_resources.index_buffer_memory);

  copyBuffer(devices,
             command_pool,
             queues,
             stagingBuffer,
             model_resources.index_buffer,
             bufferSize);

  vkDestroyBuffer(devices.logical_device, stagingBuffer, nullptr);
  vkFreeMemory(devices.logical_device, stagingBufferMemory, nullptr);

  std::cout << "Index buffer creation process ends with success..." << std::endl;
}

#endif  // SLIMEMAID_MODELLOADER_HPP
