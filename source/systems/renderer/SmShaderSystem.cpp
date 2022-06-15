/*
------------------------------------
 Slimemaid Source Code (15.06.2022)
 This file is part of Slimemaid Source Code.
----------------------------------
*/

#include "systems/renderer/SmShaderSystem.hpp"

VkShaderModule create_shader_module(SmDevices input_devices,
                                    const std::vector<char>& code) {
  VkShaderModuleCreateInfo shader_create_info{};
  shader_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shader_create_info.codeSize = code.size();
  shader_create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

  VkShaderModule shader_module;
  if (VK_SUCCESS != vkCreateShaderModule(input_devices.logical_device,
                                         &shader_create_info,
                                         nullptr,
                                         &shader_module)) {
    throw std::runtime_error("Failed to create shader module");
  }

  return shader_module;
}
