/*
------------------------------------
  Slimemaid Source Code (09.06.2022)
  This file is part of Slimemaid Source Code.
  Queue families system get access to family queue indices. Indices point to queue members. Queue member is sequence of
  steps of commands that device should execute.
------------------------------------
*/

#ifndef SLIMEMAID_SMQUEUEFAMILIESSYSTEM_HPP
#define SLIMEMAID_SMQUEUEFAMILIESSYSTEM_HPP

#include <vulkan/vulkan.h>

#include <vector>

#include "components/renderer/SmQueueFamilyIndices.hpp"
#include "components/renderer/SmSurface.hpp"

SmQueueFamilyIndices find_transfer_queue_families(VkPhysicalDevice input_device,
                                                  SmSurface input_surface);

#endif  // SLIMEMAID_SMQUEUEFAMILIESSYSTEM_HPP
