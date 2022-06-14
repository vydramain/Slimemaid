/*
------------------------------------
  Slimemaid Source Code (13.06.2022)
  This file is part of Slimemaid Source Code.

------------------------------------
*/

#ifndef SLIMEMAID_SMFRAMEBUFFERSYSTEM_HPP
#define SLIMEMAID_SMFRAMEBUFFERSYSTEM_HPP

#include <vulkan/vulkan.h>

#include <array>
#include <iostream>

#include "systems/renderer/SmImageViewSystem.hpp"
#include "systems/renderer/SmTextureImageSystem.hpp"

#include "components/renderer/SmColorImage.hpp"
#include "components/renderer/SmDepthBuffers.hpp"
#include "components/renderer/SmDevices.hpp"
#include "components/renderer/SmGraphicsPipeline.hpp"
#include "components/renderer/SmSamplingFlags.hpp"
#include "components/renderer/SmSwapChain.hpp"

void create_color_resources(SmDevices input_devices,
                          SmSwapChain input_swap_chain,
                          SmSamplingFlags input_msaa_samples,
                          SmColorImage* p_color_image) {
  VkFormat colorFormat = input_swap_chain.swap_chain_image_format;

  create_image(input_devices,
               input_swap_chain.swap_chain_extent.width,
               input_swap_chain.swap_chain_extent.height,
               1,
               input_msaa_samples.msaa_samples,
               colorFormat, VK_IMAGE_TILING_OPTIMAL,
               VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
               p_color_image->color_image,
               p_color_image->color_image_memory);
  p_color_image->color_image_view = create_image_view(input_devices,
                                                      p_color_image->color_image,
                                                      colorFormat,
                                                      VK_IMAGE_ASPECT_COLOR_BIT,
                                                      1);
}

void create_frame_buffers(SmDevices input_devices,
                          SmGraphicsPipeline input_graphics_pipeline,
                          SmColorImage input_color_image,
                          SmDepthBuffers input_depth_buffers,
                          SmSwapChain* p_swap_chain) {
  p_swap_chain->swap_chain_frame_buffers.resize(p_swap_chain->swap_chain_image_views.size());
  for (size_t i = 0; i < p_swap_chain->swap_chain_image_views.size(); i++) {
    std::array<VkImageView, 3> attachments = {
        input_color_image.color_image_view,
        input_depth_buffers.depth_image_view,
        p_swap_chain->swap_chain_image_views[i],
    };

    VkFramebufferCreateInfo frame_buffer_create_info{};
      frame_buffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      frame_buffer_create_info.renderPass = input_graphics_pipeline.render_pass;
      frame_buffer_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
      frame_buffer_create_info.pAttachments = attachments.data();
      frame_buffer_create_info.width = p_swap_chain->swap_chain_extent.width;
      frame_buffer_create_info.height = p_swap_chain->swap_chain_extent.height;
      frame_buffer_create_info.layers = 1;

    if (VK_SUCCESS != vkCreateFramebuffer(input_devices.logical_device,
                                          &frame_buffer_create_info,
                                          nullptr,
                                          &p_swap_chain->swap_chain_frame_buffers[i])) {
      throw std::runtime_error("Failed to create framebuffer");
    }
  }

  std::cout << "Frame buffers creation and implementation processs ends with success..." << std::endl;
}

#endif  // SLIMEMAID_SMFRAMEBUFFERSYSTEM_HPP
