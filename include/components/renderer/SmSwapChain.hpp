/*
------------------------------------
  Slimemaid Source Code (22.05.2022)
  This file is part of Slimemaid Source Code.
  Vulkan infrastructure that will own the render buffers before visualization on the screen.
  This infrastructure is the swap chain and must be created explicitly in Vulkan.
  The swap chain is essentially a queue of images that are waiting to be presented to the screen.
------------------------------------
*/

#ifndef SLIMEMAID_SMSWAPCHAIN_HPP
#define SLIMEMAID_SMSWAPCHAIN_HPP

#include <vulkan/vulkan.h>

#include <vector>

struct SmSwapChain {
  VkSwapchainKHR swap_chain;
  std::vector<VkImage> swap_chain_images;
  std::vector<VkImageView> swap_chain_image_views;

  VkFormat swap_chain_image_format;
  VkExtent2D swap_chain_extent;
  std::vector<VkFramebuffer> swap_chain_frame_buffers;
};

#endif  // SLIMEMAID_SMSWAPCHAIN_HPP
