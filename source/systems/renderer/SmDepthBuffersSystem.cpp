
#include "systems/renderer/SmDepthBuffersSystem.hpp"

#include <iostream>

#include "components/renderer/SmSwapChain.hpp"
#include "components/renderer/SmDepthBuffers.hpp"
#include "components/renderer/SmSamplingFlags.hpp"

#include "systems/renderer/SmImageViewSystem.hpp"
#include "systems/renderer/SmTextureImageSystem.hpp"

VkFormat find_supported_depth_format(SmDevices input_devices,
                                     const std::vector<VkFormat>& candidates,
                                     VkImageTiling input_tiling,
                                     VkFormatFeatureFlags input_flags) {
  for (VkFormat format : candidates) {
    VkFormatProperties device_properties;
    vkGetPhysicalDeviceFormatProperties(input_devices.physical_device,
                                        format,
                                        &device_properties);

    if (VK_IMAGE_TILING_LINEAR == input_tiling && (device_properties.linearTilingFeatures & input_flags) == input_flags) {
      return format;
    } else if (VK_IMAGE_TILING_OPTIMAL == input_tiling &&
               (device_properties.optimalTilingFeatures & input_flags) == input_flags) {
      return format;
    }
  }

  throw std::runtime_error("Failed to find supported depth format");
}

VkFormat find_depth_format(SmDevices input_devices) {
  return find_supported_depth_format(input_devices,
                                     {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                                     VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

void create_depth_resources(SmDevices input_devices,
                          SmQueues input_queues,
                          SmSamplingFlags input_msaa_samples,
                          SmSwapChain* p_swap_chain,
                          SmCommandPool* p_command_pool,
                          SmDepthBuffers* p_depth_buffers) {
  VkFormat depthFormat = find_depth_format(input_devices);

  create_image(input_devices,
               p_swap_chain->swap_chain_extent.width,
               p_swap_chain->swap_chain_extent.height,
               1,
               input_msaa_samples.msaa_samples,
               depthFormat,
               VK_IMAGE_TILING_OPTIMAL,
               VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
               p_depth_buffers->depth_image,
               p_depth_buffers->depth_image_memory);

  p_depth_buffers->depth_image_view = create_image_view(input_devices,
                                                     p_depth_buffers->depth_image,
                                                     depthFormat,
                                                     VK_IMAGE_ASPECT_DEPTH_BIT,
                                                     1);
  transition_image_layout(input_devices,
                          p_command_pool->command_pool,
                          input_queues.graphics_queue,
                          p_depth_buffers->depth_image,
                          depthFormat,
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                          1);

  std::cout << "Depth resources creation process ends with success..." << std::endl;
}
