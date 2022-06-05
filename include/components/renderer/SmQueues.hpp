/*
------------------------------------
  Slimemaid Source Code (05.06.2022)
  This file is part of Slimemaid Source Code.
  Vulkan's logical device also creates the queues associated with that device. Component keeps necessary queues:
    - Graphics queue is command queue for rendering.
    - To establish the connection between Vulkan and the window system to present results to the screen,
      use the WSI (Window System Integration) extensions. Present queue is command queue for this system.
------------------------------------
*/

#ifndef SLIMEMAID_SMQUEUES_HPP
#define SLIMEMAID_SMQUEUES_HPP

#include <vulkan/vulkan.h>

struct SmQueues {
  VkQueue graphics_queue{};
  VkQueue present_queue{};
};

#endif  // SLIMEMAID_SMQUEUES_HPP
