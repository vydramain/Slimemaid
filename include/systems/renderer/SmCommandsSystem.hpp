/*
------------------------------------
  Slimemaid Source Code (22.05.2022)
  This file is part of Slimemaid Source Code.
------------------------------------
*/

#ifndef SLIMEMAID_SMCOMMANDSSYSTEM_HPP
#define SLIMEMAID_SMCOMMANDSSYSTEM_HPP

#include <vulkan/vulkan.h>

VkCommandBuffer beginSingleTimeCommands(VkDevice inputDevice, VkCommandPool inputCommandPool) {
  VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
  commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  commandBufferAllocateInfo.commandPool = inputCommandPool;
  commandBufferAllocateInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(inputDevice, &commandBufferAllocateInfo, &commandBuffer);

  VkCommandBufferBeginInfo commandBufferBeginInfo{};
  commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

  return commandBuffer;
}

void endSingleTimeCommands(VkDevice inputDevice,
                           VkCommandPool inputCommandPool,
                           VkQueue inputGraphicsQueue,
                           VkCommandBuffer inputCommandBuffer) {
  vkEndCommandBuffer(inputCommandBuffer);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &inputCommandBuffer;

  vkQueueSubmit(inputGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(inputGraphicsQueue);

  vkFreeCommandBuffers(inputDevice, inputCommandPool, 1, &inputCommandBuffer);
}

#endif  // SLIMEMAID_SMCOMMANDSSYSTEM_HPP
