/*
------------------------------------
  Slimemaid Source Code (13.06.2022)
  This file is part of Slimemaid Source Code.

------------------------------------
*/

#ifndef SLIMEMAID_SMDESCRIPTORSSYSTEM_HPP
#define SLIMEMAID_SMDESCRIPTORSSYSTEM_HPP

#include <vulkan/vulkan.h>

#include <array>
#include <iostream>
#include <stdexcept>

#include "components/renderer/SmDevices.hpp"
#include "components/renderer/SmDescriptorPool.hpp"

void createDescriptorSetLayout(SmDevices input_devices,
                               SmDescriptorPool* p_descriptor_pool) {
  VkDescriptorSetLayoutBinding uboLayoutBinding{};
  uboLayoutBinding.binding = 0;
  uboLayoutBinding.descriptorCount = 1;
  uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding.pImmutableSamplers = nullptr;
  uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayoutBinding samplerLayoutBinding{};
  samplerLayoutBinding.binding = 1;
  samplerLayoutBinding.descriptorCount = 1;
  samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerLayoutBinding.pImmutableSamplers = nullptr;
  samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};
  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
  layoutInfo.pBindings = bindings.data();

  if (VK_SUCCESS != vkCreateDescriptorSetLayout(input_devices.logical_device, &layoutInfo, nullptr,
                                                &p_descriptor_pool->descriptor_set_layout)) {
    throw std::runtime_error("Failed to create descriptor set layout");
  }

  std::cout << "Set layout process description creation ends with success..." << std::endl;
}

#endif  // SLIMEMAID_SMDESCRIPTORSSYSTEM_HPP
