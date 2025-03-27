
#include "systems/renderer/SmFrameBufferSystem.hpp"

#include <format>
#include <iostream>

#include "components/renderer/SmGraphicsPipeline.hpp"
#include "systems/renderer/SmImageViewSystem.hpp"
#include "systems/renderer/SmTextureImageSystem.hpp"

bool check_device_graphics_queue_support(SmDevices devices) {
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(devices.physical_device,
                                           &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(
      devices.physical_device, &queueFamilyCount, queueFamilies.data());

  for (uint32_t i = 0; i < queueFamilyCount; i++) {
    if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      std::cout << "Queue family " << i << " for device "
                << devices.get_device_name()
                << " supports VK_QUEUE_GRAPHICS_BIT\n";
      return true;  // Found a queue family with graphics support
    }
  }
  return false;  // No queue family supports graphics
}

void sl_create_color_resources(SmDevices input_devices,
                            SmSwapChain input_swap_chain,
                            SmSamplingFlags input_msaa_samples,
                            SmColorImage* p_color_image) {
  VkFormat colorFormat = input_swap_chain.swap_chain_image_format;

  sl_create_image(input_devices, input_swap_chain.swap_chain_extent.width,
               input_swap_chain.swap_chain_extent.height, 1,
               input_msaa_samples.msaa_samples, colorFormat,
               VK_IMAGE_TILING_OPTIMAL,
               VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
                   VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, p_color_image->color_image,
               p_color_image->color_image_memory);
  p_color_image->color_image_view =
      sl_create_image_view(input_devices, p_color_image->color_image, colorFormat,
                        VK_IMAGE_ASPECT_COLOR_BIT, 1);

  std::cout << "Color resources creation processs ends with success..."
            << std::endl;
}

void sl_create_frame_buffers(SmDevices input_devices,
                          SmGraphicsPipeline input_graphics_pipeline,
                          SmColorImage input_color_image,
                          SmDepthBuffers input_depth_buffers,
                          SmSwapChain* p_swap_chain) {
  std::cout << "Frame buffers creation process starts..." << std::endl;
  p_swap_chain->swap_chain_frame_buffers.resize(
      p_swap_chain->swap_chain_image_views.size());
  std::cout << "New swap chain size is: "
            << p_swap_chain->swap_chain_frame_buffers.size() << std::endl;

  for (size_t i = 0; i < p_swap_chain->swap_chain_image_views.size(); i++) {
    std::cout << "Check access to color_image_view: `"
              << input_color_image.color_image_view << "`" << std::endl;
    std::cout << "Check access to depth_image_view: `"
              << input_depth_buffers.depth_image_view << "`" << std::endl;
    std::cout << "Check access to swap_chain_image_views: `"
              << p_swap_chain->swap_chain_image_views[i] << "`" << std::endl;

    VkImageView attachments[] = {
        input_color_image.color_image_view,
        input_depth_buffers.depth_image_view,
        p_swap_chain->swap_chain_image_views[i],
    };

    VkFramebufferCreateInfo frame_buffer_create_info{};
    frame_buffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frame_buffer_create_info.renderPass = input_graphics_pipeline.render_pass;
    frame_buffer_create_info.attachmentCount =
        static_cast<uint32_t>(std::size(attachments));
    frame_buffer_create_info.pAttachments = attachments;
    frame_buffer_create_info.width = p_swap_chain->swap_chain_extent.width;
    frame_buffer_create_info.height = p_swap_chain->swap_chain_extent.height;
    frame_buffer_create_info.layers = 1;

    std::cout << "Frame buffer info created..." << std::endl;
    if (check_device_graphics_queue_support(input_devices) == false) {
      throw std::runtime_error(
          std::format("Failed to create frame buffer for device: {}, because "
                      "this device doesn't support VK_QUEUE_GRAPHICS_BIT",
                      input_devices.get_device_name()));
    }

    if (VK_SUCCESS !=
        vkCreateFramebuffer(input_devices.logical_device,
                            &frame_buffer_create_info, nullptr,
                            &p_swap_chain->swap_chain_frame_buffers[i])) {
      throw std::runtime_error("Failed to create frame buffer");
    }
  }

  std::cout << "Frame buffers creation and implementation processs ends with "
               "success..."
            << std::endl;
}
