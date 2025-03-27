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
#include "components/renderer/SmSurface.hpp"
#include "components/renderer/SmSwapChain.hpp"
#include "components/renderer/SmColorImage.hpp"
#include "components/renderer/SmGLFWWindow.hpp"
#include "components/renderer/SmDepthBuffers.hpp"
#include "components/renderer/SmGraphicsPipeline.hpp"
#include "components/renderer/SmSwapChainSupportDetails.hpp"

void sl_clean_up_swap_chain(SmDevices input_devices,
                         SmColorImage input_color_image,
                         SmDepthBuffers input_depth_buffers,
                         SmGraphicsPipeline input_graphics_pipeline,
                         SmSwapChain* p_swap_chain);

SmSwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice input_device,
                                                   SmSurface input_surface);

VkSurfaceFormatKHR sl_choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats);

VkPresentModeKHR sl_choose_swap_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes);

VkExtent2D sl_choose_swap_extent(SmGLFWWindow input_window,
                              const VkSurfaceCapabilitiesKHR& capabilities);

void sl_create_swap_chain(SmDevices input_devices,
                       SmSurface input_surface,
                       SmGLFWWindow input_window,
                       SmSwapChain* swap_chain);

void sl_create_image_views(SmDevices input_devices,
                        SmSwapChain* p_swap_chain);

#endif  // SLIMEMAID_SMSWAPCHAINSYSTEM_H
