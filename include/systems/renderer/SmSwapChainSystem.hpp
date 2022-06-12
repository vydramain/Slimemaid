/*
------------------------------------
  Slimemaid Source Code (09.06.2022)
  This file is part of Slimemaid Source Code.
  Vulkan infrastructure own the render buffers that have whole ready image before visualization on the screen.
  Swap chain system create this infrastructure called swap chain explicitly as it requires Vulkan for every special
  graphics card that support swap chains.
------------------------------------
*/

#ifndef SLIMEMAID_SMSWAPCHAINSYSTEM_H
#define SLIMEMAID_SMSWAPCHAINSYSTEM_H

#include <vulkan/vulkan.h>

#include "components/renderer/SmDevices.hpp"
#include "components/renderer/SmGLFWWindow.hpp"
#include "components/renderer/SmSurface.hpp"
#include "components/renderer/SmSwapChain.hpp"
#include "components/renderer/SmSwapChainSupportDetails.hpp"

#include "systems/renderer/SmImageViewSystem.hpp"
#include "systems/renderer/SmGLFWWindowSystem.hpp"
#include "systems/renderer/SmQueueFamiliesSystem.hpp"
#include "systems/renderer/SmGraphicsPipelineSystem.hpp"

void clean_up_swap_chain(SmDevices input_devices,
                         SmColorImage input_color_image,
                         SmDepthBuffers input_depth_buffers,
                         SmGraphicsPipeline input_graphics_pipeline,
                         SmSwapChain* p_swap_chain) {
  vkDestroyImageView(input_devices.logical_device, input_color_image.color_image_view, nullptr);
  vkDestroyImage(input_devices.logical_device, input_color_image.color_image, nullptr);
  vkFreeMemory(input_devices.logical_device, input_color_image.color_image_memory, nullptr);

  vkDestroyImageView(input_devices.logical_device, input_depth_buffers.depth_image_view, nullptr);  // dif
  vkDestroyImage(input_devices.logical_device, input_depth_buffers.depth_image, nullptr);           // dif
  vkFreeMemory(input_devices.logical_device, input_depth_buffers.depth_image_memory, nullptr);      // dif

  for (auto framebuffer : p_swap_chain->swap_chain_frame_buffers) {
    vkDestroyFramebuffer(input_devices.logical_device, framebuffer, nullptr);
  }

  vkDestroyPipeline(input_devices.logical_device, input_graphics_pipeline.pipeline, nullptr);
  vkDestroyPipelineLayout(input_devices.logical_device, input_graphics_pipeline.pipeline_layout, nullptr);
  vkDestroyRenderPass(input_devices.logical_device, input_graphics_pipeline.render_pass, nullptr);

  for (auto imageView : p_swap_chain->swap_chain_image_views) {
    vkDestroyImageView(input_devices.logical_device, imageView, nullptr);
  }

  vkDestroySwapchainKHR(input_devices.logical_device, p_swap_chain->swap_chain, nullptr);

  std::cout << "Swap chain clean up process ends with success..." << std::endl;
}

SmSwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice input_device,
                                                   SmSurface input_surface) {
  SmSwapChainSupportDetails details;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(input_device,
                                            input_surface.surface_khr,
                                            &details.capabilities);

  uint32_t format_count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(input_device,
                                       input_surface.surface_khr,
                                       &format_count,
                                       nullptr);
  if (0 != format_count) {
    details.formats.resize(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(input_device,
                                         input_surface.surface_khr,
                                         &format_count,
                                         details.formats.data());
  }

  uint32_t present_mode_count;
  vkGetPhysicalDeviceSurfacePresentModesKHR(input_device,
                                            input_surface.surface_khr,
                                            &present_mode_count,
                                            nullptr);

  if (0 != present_mode_count) {
    details.present_modes.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(input_device,
                                              input_surface.surface_khr,
                                              &present_mode_count,
                                              details.present_modes.data());
  }

  return details;
}

VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats) {
  for (const auto& available_format : available_formats) {
    if (VK_FORMAT_B8G8R8A8_SRGB == available_format.format &&
        VK_COLORSPACE_SRGB_NONLINEAR_KHR == available_format.colorSpace) {
      return available_format;
    }
  }
  return available_formats[0];
}

VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes) {
  for (const auto& available_present_mode : available_present_modes) {
    if (VK_PRESENT_MODE_MAILBOX_KHR == available_present_mode) {
      return available_present_mode;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D choose_swap_extent(SmGLFWWindow input_window,
                              const VkSurfaceCapabilitiesKHR& capabilities) {
  if (std::numeric_limits<uint32_t>::max() != capabilities.currentExtent.width) {
    return capabilities.currentExtent;
  } else {
    int height;
    int width;
    get_frame_buffer_size(input_window, width, height);

    VkExtent2D actual_extent = {static_cast<uint32_t>(height), static_cast<uint32_t>(width)};

    actual_extent.width = std::clamp(actual_extent.width,
                                     capabilities.minImageExtent.width,
                                     capabilities.maxImageExtent.width);
    actual_extent.height = std::clamp(actual_extent.height,
                                      capabilities.minImageExtent.height,
                                      capabilities.maxImageExtent.height);

    return actual_extent;
  }
}

void create_swap_chain(SmDevices input_devices,
                       SmSurface input_surface,
                       SmGLFWWindow input_window,
                       SmSwapChain* swap_chain) {
  SmSwapChainSupportDetails swap_chain_support = query_swap_chain_support(input_devices.physical_device,
                                                                          input_surface);

  VkSurfaceFormatKHR surface_format = choose_swap_surface_format(swap_chain_support.formats);
  VkPresentModeKHR present_mode = choose_swap_present_mode(swap_chain_support.present_modes);
  VkExtent2D tmp_swap_chain_extent = choose_swap_extent(input_window, swap_chain_support.capabilities);

  uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;
  if (swap_chain_support.capabilities.maxImageCount > 0 &&
      image_count > swap_chain_support.capabilities.maxImageCount) {
    image_count = swap_chain_support.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  create_info.surface = input_surface.surface_khr;
  create_info.minImageCount = image_count;
  create_info.imageFormat = surface_format.format;
  create_info.imageColorSpace = surface_format.colorSpace;
  create_info.imageExtent = tmp_swap_chain_extent;
  create_info.imageArrayLayers = 1;
  create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  SmQueueFamilyIndices indices = find_transfer_queue_families(input_devices.physical_device,
                                                              input_surface);
  uint32_t queue_family_indices[] = {indices.graphics_family.value(), indices.present_family.value()};

  if (indices.graphics_family != indices.present_family) {
    create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    create_info.queueFamilyIndexCount = 2;
    create_info.pQueueFamilyIndices = queue_family_indices;
  } else {
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0;      // Optional
    create_info.pQueueFamilyIndices = nullptr;  // Optional
  }

  create_info.preTransform = swap_chain_support.capabilities.currentTransform;
  create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  create_info.presentMode = present_mode;
  create_info.clipped = VK_TRUE;

  if (VK_SUCCESS != vkCreateSwapchainKHR(input_devices.logical_device,
                                         &create_info,
                                         nullptr,
                                         &swap_chain->swap_chain)) {
    throw std::runtime_error("Failed to create swap chain");
  }

  vkGetSwapchainImagesKHR(input_devices.logical_device,
                          swap_chain->swap_chain,
                          &image_count,
                          nullptr);
  swap_chain->swap_chain_images.resize(image_count);
  vkGetSwapchainImagesKHR(input_devices.logical_device,
                          swap_chain->swap_chain,
                          &image_count,
                          swap_chain->swap_chain_images.data());

  swap_chain->swap_chain_image_format = surface_format.format;
  swap_chain->swap_chain_extent = tmp_swap_chain_extent;

  std::cout << "Swap chain creation process ends with success..." << std::endl;
}

void create_image_views(SmDevices input_devices,
                        SmSwapChain* p_swap_chain) {
  p_swap_chain->swap_chain_image_views.resize(p_swap_chain->swap_chain_images.size());

  for (uint32_t i = 0; i < p_swap_chain->swap_chain_images.size(); i++) {
    p_swap_chain->swap_chain_image_views[i] = create_image_view(input_devices,
                                                                p_swap_chain->swap_chain_images[i],
                                                                p_swap_chain->swap_chain_image_format,
                                                                VK_IMAGE_ASPECT_COLOR_BIT,
                                                                1);
  }

  std::cout << "Image view creation process ends with success..." << std::endl;
}

#endif  // SLIMEMAID_SMSWAPCHAINSYSTEM_H
