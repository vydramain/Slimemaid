/*
------------------------------------
  Slimemaid Source Code (04.06.2022)
  This file is part of Slimemaid Source Code.
  Component keeps object for access to command pool for graphics device. Command buffers count equal to frame
  that rendering on flight:
    - Command pools are opaque objects that command buffer memory is allocated from,
      and which allow the implementation to amortize the cost of resource creation across multiple command buffers.
    - Command buffers are objects used to record commands which can be subsequently submitted to
      a device queue for execution. There are two levels of command buffers - primary command buffers,
      which can execute secondary command buffers, and which are submitted to queues, and secondary command buffers,
      which can be executed by primary command buffers, and which are not directly submitted to queues.
------------------------------------
*/

#ifndef SLIMEMAID_SMCOMMANDPOOL_HPP
#define SLIMEMAID_SMCOMMANDPOOL_HPP

#include <vulkan/vulkan.h>

#include <cstdint> // Necessary for uint32_t
#include <vector>

struct SmCommandPool {
  const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

  VkCommandPool commandPool;
  std::vector<VkCommandBuffer> commandBuffers;
};

#endif  // SLIMEMAID_SMCOMMANDPOOL_HPP
