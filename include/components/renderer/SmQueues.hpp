/*
------------------------------------
  Slimemaid Source Code (22.05.2022)
  This file is part of Slimemaid Source Code.
  Component with device information. Vulkan have physic and virtual devices.
  These devices provide access to memory and commands queue and pools.
------------------------------------
*/

#ifndef SLIMEMAID_SMQUEUES_HPP
#define SLIMEMAID_SMQUEUES_HPP

#include <vulkan/vulkan.h>

struct SmQueues {
  VkQueue graphics_queue;
  VkQueue present_queue;
};

#endif  // SLIMEMAID_SMQUEUES_HPP
