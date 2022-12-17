/*
------------------------------------
  Slimemaid Source Code (04.06.2022)
  This file is part of Slimemaid Source Code.
  Graphics pipeline system can fill appropriate component by input shaders and input settings.
------------------------------------
*/

#ifndef SLIMEMAID_SMGRAPHICSPIPELINESYSTEM_HPP
#define SLIMEMAID_SMGRAPHICSPIPELINESYSTEM_HPP

#include <vulkan/vulkan.h>

#include <array>
#include <iostream>
#include <stdexcept>

#include "components/renderer/SmDescriptorPool.hpp"
#include "components/renderer/SmDevices.hpp"
#include "components/renderer/SmGraphicsPipeline.hpp"
#include "components/renderer/SmSamplingFlags.hpp"
#include "components/renderer/SmSwapChain.hpp"
#include "components/renderer/SmVertex.hpp"

#include "systems/reader/SmFileReaderSystem.hpp"

#include "systems/renderer/SmDepthBuffersSystem.hpp"
#include "systems/renderer/SmShaderSystem.hpp"

void create_graphics_pipeline(std::string input_vertex_shader_path,
                              std::string input_fragment_shader_path,
                              SmDevices input_devices,
                              SmSwapChain* p_swap_chain,
                              SmDescriptorPool* p_descriptor_pool,
                              SmGraphicsPipeline* p_graphics_pipeline,
                              VkSampleCountFlagBits input_msaa_samples);

void create_render_pass(SmDevices input_devices,
                        SmSamplingFlags input_msaa_samples,
                        SmSwapChain* p_swap_chain,
                        SmGraphicsPipeline* p_graphics_pipeline);

#endif  // SLIMEMAID_SMGRAPHICSPIPELINESYSTEM_HPP
