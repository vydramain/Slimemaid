/*
------------------------------------
  Slimemaid Source Code (04.06.2022)
  This file is part of Slimemaid Source Code.
------------------------------------
*/

#ifndef SLIMEMAID_SMSHADERSYSTEM_HPP
#define SLIMEMAID_SMSHADERSYSTEM_HPP

#include <vulkan/vulkan.h>

#include <vector>
#include <fstream>

#include "components/renderer/SmDevices.hpp"

VkShaderModule createShaderModule(SmDevices input_devices, const std::vector<char>& code) {
  VkShaderModuleCreateInfo shaderCreateInfo{};
  shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shaderCreateInfo.codeSize = code.size();
  shaderCreateInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

  VkShaderModule shaderModule;
  if (VK_SUCCESS != vkCreateShaderModule(input_devices.device, &shaderCreateInfo, nullptr, &shaderModule)) {
    throw std::runtime_error("Failed to create shader module");
  }

  return shaderModule;
}

#endif  // SLIMEMAID_SMSHADERSYSTEM_HPP
