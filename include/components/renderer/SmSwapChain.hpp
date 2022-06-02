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
  VkSwapchainKHR swapChain;
  std::vector<VkImage> swapChainImages;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;
  std::vector<VkImageView> swapChainImageViews;
  std::vector<VkFramebuffer> swapChainFramebuffers;
};

#endif  // SLIMEMAID_SMSWAPCHAIN_HPP
