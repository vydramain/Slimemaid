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

#include <string>

#include "components/renderer/SmDescriptorPool.hpp"
#include "components/renderer/SmDevices.hpp"
#include "components/renderer/SmGraphicsPipeline.hpp"
#include "components/renderer/SmSamplingFlags.hpp"
#include "components/renderer/SmSwapChain.hpp"

void create_graphics_pipeline(const std::string& input_vertex_shader_path,
                              const std::string& input_fragment_shader_path, SmDevices& devices,
                              const SmSwapChain& input_swap_chain,
                              SmDescriptorPool& descriptor_pool,
                              SmGraphicsPipeline& graphics_pipeline,
                              VkSampleCountFlagBits input_msaa_samples);

void create_render_pass(SmDevices input_devices, SmSamplingFlags input_msaa_samples,
                        SmGraphicsPipeline* p_graphics_pipeline, SmSwapChain* p_swap_chain);

#endif  // SLIMEMAID_SMGRAPHICSPIPELINESYSTEM_HPP
