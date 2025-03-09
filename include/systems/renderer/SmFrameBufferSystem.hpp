/*
------------------------------------
  Slimemaid Source Code (13.06.2022)
  This file is part of Slimemaid Source Code.

------------------------------------
*/

#ifndef SLIMEMAID_SMFRAMEBUFFERSYSTEM_HPP
#define SLIMEMAID_SMFRAMEBUFFERSYSTEM_HPP

#include <vulkan/vulkan.h>

#include "components/renderer/SmDevices.hpp"
#include "components/renderer/SmSwapChain.hpp"
#include "components/renderer/SmColorImage.hpp"
#include "components/renderer/SmDepthBuffers.hpp"
#include "components/renderer/SmSamplingFlags.hpp"
#include "components/renderer/SmGraphicsPipeline.hpp"

void create_color_resources(SmDevices input_devices,
                          SmSwapChain input_swap_chain,
                          SmSamplingFlags input_msaa_samples,
                          SmColorImage* p_color_image);

void create_frame_buffers(SmDevices input_devices,
                          SmGraphicsPipeline input_graphics_pipeline,
                          SmColorImage input_color_image,
                          SmDepthBuffers input_depth_buffers,
                          SmSwapChain* p_swap_chain);

#endif  // SLIMEMAID_SMFRAMEBUFFERSYSTEM_HPP
