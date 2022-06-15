/*
------------------------------------
 Slimemaid Source Code (15.06.2022)
 This file is part of Slimemaid Source Code.
----------------------------------
*/

#include "systems/renderer/SmDescriptorsSystem.hpp"

void create_descriptor_set_layout(SmDevices input_devices,
                                  SmDescriptorPool* p_descriptor_pool) {
  VkDescriptorSetLayoutBinding ubo_layout_binding{};
  ubo_layout_binding.binding = 0;
  ubo_layout_binding.descriptorCount = 1;
  ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  ubo_layout_binding.pImmutableSamplers = nullptr;
  ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayoutBinding sampler_layout_binding{};
  sampler_layout_binding.binding = 1;
  sampler_layout_binding.descriptorCount = 1;
  sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  sampler_layout_binding.pImmutableSamplers = nullptr;
  sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  std::array<VkDescriptorSetLayoutBinding, 2> bindings = {ubo_layout_binding, sampler_layout_binding};
  VkDescriptorSetLayoutCreateInfo layout_info{};
  layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
  layout_info.pBindings = bindings.data();

  if (VK_SUCCESS != vkCreateDescriptorSetLayout(input_devices.logical_device,
                                                &layout_info,
                                                nullptr,
                                                &p_descriptor_pool->descriptor_set_layout)) {
    throw std::runtime_error("Failed to create descriptor set layout");
  }

  std::cout << "Set layout process description creation ends with success..." << std::endl;
}

void create_descriptor_pool(SmDevices input_devices,
                            SmCommandPool* p_command_pool,
                            SmDescriptorPool* p_descriptor_pool) {
  std::array<VkDescriptorPoolSize, 2> descriptor_pool_sizes{};
  descriptor_pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptor_pool_sizes[0].descriptorCount = static_cast<uint32_t>(p_command_pool->MAX_FRAMES_IN_FLIGHT);
  descriptor_pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  descriptor_pool_sizes[1].descriptorCount = static_cast<uint32_t>(p_command_pool->MAX_FRAMES_IN_FLIGHT);

  VkDescriptorPoolCreateInfo descriptor_pool_create_info{};
  descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptor_pool_create_info.poolSizeCount = static_cast<uint32_t>(descriptor_pool_sizes.size());
  descriptor_pool_create_info.pPoolSizes = descriptor_pool_sizes.data();
  descriptor_pool_create_info.maxSets = static_cast<uint32_t>(p_command_pool->MAX_FRAMES_IN_FLIGHT);

  if (VK_SUCCESS != vkCreateDescriptorPool(input_devices.logical_device,
                                           &descriptor_pool_create_info,
                                           nullptr,
                                           &p_descriptor_pool->descriptor_pool)) {
    throw std::runtime_error("Failed to create descriptor pool for uniform buffers");
  }

  std::cout << "Description pool creation process ends with success..." << std::endl;
}

void create_descriptor_sets(SmDevices input_devices,
                            SmTextureImageViewSampler input_texture_model_resources_read_handler,
                            SmUniformBuffers* p_uniform_buffers,
                            SmCommandPool* p_command_pool,
                            SmDescriptorPool* p_descriptor_pool) {
  std::vector<VkDescriptorSetLayout> layouts(p_command_pool->MAX_FRAMES_IN_FLIGHT,
                                             p_descriptor_pool->descriptor_set_layout);
  VkDescriptorSetAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = p_descriptor_pool->descriptor_pool;
  alloc_info.descriptorSetCount = static_cast<uint32_t>(p_command_pool->MAX_FRAMES_IN_FLIGHT);
  alloc_info.pSetLayouts = layouts.data();

  p_descriptor_pool->descriptor_sets.resize(p_command_pool->MAX_FRAMES_IN_FLIGHT);
  if (vkAllocateDescriptorSets(input_devices.logical_device,
                               &alloc_info,
                               p_descriptor_pool->descriptor_sets.data()) != VK_SUCCESS) {
    throw std::runtime_error("Failed to allocate descriptor sets for uniform buffers");
  }

  for (size_t i = 0; i < p_command_pool->MAX_FRAMES_IN_FLIGHT; i++) {
    VkDescriptorBufferInfo buffer_info{};
    buffer_info.buffer = p_uniform_buffers->uniform_buffers[i];
    buffer_info.offset = 0;
    buffer_info.range = sizeof(SmUniformBufferObject);

    VkDescriptorImageInfo image_info{};
    image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_info.imageView = input_texture_model_resources_read_handler.texture_image_view;
    image_info.sampler = input_texture_model_resources_read_handler.texture_sampler;

    std::array<VkWriteDescriptorSet, 2> descriptor_writes{};
    descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_writes[0].dstSet = p_descriptor_pool->descriptor_sets[i];
    descriptor_writes[0].dstBinding = 0;
    descriptor_writes[0].dstArrayElement = 0;
    descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor_writes[0].descriptorCount = 1;
    descriptor_writes[0].pBufferInfo = &buffer_info;

    descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_writes[1].dstSet = p_descriptor_pool->descriptor_sets[i];
    descriptor_writes[1].dstBinding = 1;
    descriptor_writes[1].dstArrayElement = 0;
    descriptor_writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor_writes[1].descriptorCount = 1;
    descriptor_writes[1].pImageInfo = &image_info;

    vkUpdateDescriptorSets(input_devices.logical_device,
                           static_cast<uint32_t>(descriptor_writes.size()),
                           descriptor_writes.data(),
                           0,
                           nullptr);
  }

  std::cout << "Descriptor sets creation process ends with success..." << std::endl;
}
