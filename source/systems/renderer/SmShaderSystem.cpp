
#include "systems/renderer/SmShaderSystem.hpp"

#include <vulkan/vulkan.h>

#include <stdexcept>
#include <vector>

#include "components/renderer/SmDevices.hpp"

VkShaderModule create_shader_module(SmDevices input_devices, const std::vector<char>& code) {
  VkShaderModuleCreateInfo shaderCreateInfo{};
  shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shaderCreateInfo.codeSize = code.size();
  shaderCreateInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

  VkShaderModule shaderModule;
  if (VK_SUCCESS != vkCreateShaderModule(input_devices.logical_device, &shaderCreateInfo, nullptr, &shaderModule)) {
    throw std::runtime_error("Failed to create shader module");
  }

  return shaderModule;
}
