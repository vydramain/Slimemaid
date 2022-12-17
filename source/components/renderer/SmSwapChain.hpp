/*
------------------------------------
  Slimemaid Source Code (22.05.2022)
  This file is part of Slimemaid Source Code.
  Present graphics cards are not capable with presenting images directly to a screen. Image presentation is heavily tied
  with the window system and the surfaces associated with windows. Vulkan core does not provide window surfaces for
  presenting images on surfaces. Swap chain is special data structure for exchange of rendered images with
  window surfaces.
  Therefore, the swap chain is essentially a queue of images that are waiting to be presented to the screen.
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
