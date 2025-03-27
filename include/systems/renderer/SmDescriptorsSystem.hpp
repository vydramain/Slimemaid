/*
------------------------------------
  Slimemaid Source Code (13.06.2022)
  This file is part of Slimemaid Source Code.

------------------------------------
*/

#ifndef SLIMEMAID_SMDESCRIPTORSSYSTEM_HPP
#define SLIMEMAID_SMDESCRIPTORSSYSTEM_HPP

#include <vulkan/vulkan.h>

#include "components/renderer/SmCommandPool.hpp"
#include "components/renderer/SmDescriptorPool.hpp"
#include "components/renderer/SmDevices.hpp"
#include "components/renderer/SmTextureImageViewSampler.hpp"
#include "components/renderer/SmUniformBuffers.hpp"

void sl_create_descriptor_set_layout(SmDevices input_devices,
                                  SmDescriptorPool* p_descriptor_pool);

void sl_create_descriptor_pool(SmDevices input_devices,
                            SmCommandPool* p_command_pool,
                            SmDescriptorPool* p_descriptor_pool);

void sl_create_descriptor_sets(SmDevices input_devices,
                            SmTextureImageViewSampler input_texture_model_resources_read_handler,
                            SmUniformBuffers* p_uniform_buffers,
                            SmCommandPool* p_command_pool,
                            SmDescriptorPool* p_descriptor_pool);

#endif  // SLIMEMAID_SMDESCRIPTORSSYSTEM_HPP
