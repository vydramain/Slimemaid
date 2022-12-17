/*
------------------------------------
  Slimemaid Source Code (04.06.2022)
  This file is part of Slimemaid Source Code.
------------------------------------
*/

#ifndef SLIMEMAID_SMSHADERSYSTEM_HPP
#define SLIMEMAID_SMSHADERSYSTEM_HPP

#include <vulkan/vulkan.h>

#include <stdexcept>
#include <vector>

#include "components/renderer/SmDevices.hpp"

VkShaderModule create_shader_module(SmDevices input_devices,
                                    const std::vector<char>& code);

#endif  // SLIMEMAID_SMSHADERSYSTEM_HPP
