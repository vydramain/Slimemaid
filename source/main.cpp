#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORSE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES  // dif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <algorithm>  // Necessary for std::clamp
#include <array>
#include <cassert>
#include <chrono>
#include <cstdint>  // Necessary for uint32_t
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>  // Necessary for std::numeric_limits
#include <map>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

#include "components/renderer/SmColorImage.hpp"
#include "components/renderer/SmCommandPool.hpp"
#include "components/renderer/SmDepthBuffers.hpp"
#include "components/renderer/SmDescriptorPool.hpp"
#include "components/renderer/SmDevices.hpp"
#include "components/renderer/SmFrame.hpp"
#include "components/renderer/SmGLFWWindow.hpp"
#include "components/renderer/SmModelResources.hpp"
#include "components/renderer/SmQueueFamilyIndices.hpp"
#include "components/renderer/SmQueues.hpp"
#include "components/renderer/SmSamplingFlags.hpp"
#include "components/renderer/SmSurface.hpp"
#include "components/renderer/SmSwapChain.hpp"
#include "components/renderer/SmSwapChainSupportDetails.hpp"
#include "components/renderer/SmTextureImage.hpp"
#include "components/renderer/SmTextureImageViewSampler.hpp"
#include "components/renderer/SmUniformBufferObject.hpp"
#include "components/renderer/SmUniformBuffers.hpp"
#include "components/renderer/SmVertex.hpp"
#include "components/renderer/SmVulkanInstance.hpp"
#include "systems/debug/SmDebugSystem.hpp"
#include "systems/renderer/SmBuffersSystem.hpp"
#include "systems/renderer/SmCommandsSystem.hpp"
#include "systems/renderer/SmDepthBuffersSystem.hpp"
#include "systems/renderer/SmDescriptorsSystem.hpp"
#include "systems/renderer/SmDeviceSystem.hpp"
#include "systems/renderer/SmGraphicsPipelineSystem.hpp"
#include "systems/renderer/SmImageViewSystem.hpp"
#include "systems/renderer/SmModelLoaderSystem.hpp"
#include "systems/renderer/SmTextureImageSystem.hpp"
#include "systems/renderer/SmVulkanInstanceSystem.hpp"

const std::string VERTEX_SHADERS_PATH = "./shaders/vert.spv";
const std::string FRAGMENT_SHADERS_PATH = "./shaders/frag.spv";

class SmVulkanRendererSystem {
 private:
  SmFrame frameParams;
  SmGLFWWindow window{};
  SmSurface surface{};
  SmVulkanInstance instance{};

  VkDebugUtilsMessengerEXT debugMessenger{};

  SmDevices devices;
  SmQueues queues;
  SmSwapChain swap_chain;
  SmDepthBuffers depth_buffers{};
  SmTextureImage texture_model_resources{};
  SmTextureImageViewSampler texture_model_resources_read_handler{};
  SmModelResources scene_model_resources;
  SmGraphicsPipeline graphics_pipeline{};
  SmColorImage color_image{};
  SmCommandPool command_pool;
  SmDescriptorPool descriptor_pool;
  SmUniformBuffers uniform_buffers;
  SmSamplingFlags msaa_samples;

  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;

  bool framebufferResized = false;
  uint32_t currentFrame = 0;

  void initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window.glfw_window = glfwCreateWindow((int) frameParams.WIDTH,
                                          (int) frameParams.HEIGHT,
                                          "Vulkan",
                                          nullptr,
                                          nullptr);
    glfwSetWindowUserPointer(window.glfw_window, this);
    glfwSetFramebufferSizeCallback(window.glfw_window, framebufferResizeCallback);
    std::cout << "GLFW initialization process ends with success..." << std::endl;
  }

  static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<SmVulkanRendererSystem*>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
  }

  void initVulkan() {
    create_instance(&instance,
                    enable_validation_layers);
    setup_debug_messenger(enable_validation_layers,
                          instance,
                          &debugMessenger);
    create_surface(window,
                   instance,
                   &surface);
    pick_physical_device(instance,
                         &msaa_samples,
                         &devices,
                         surface);
    create_logical_device(&devices,
                          surface,
                          &queues,
                          enable_validation_layers);
    create_swap_chain(devices,
                      surface,
                      window,
                      &swap_chain);
    create_image_views(devices,
                       &swap_chain);
    create_render_pass(devices,
                       msaa_samples,
                       &graphics_pipeline,
                       &swap_chain);
    createDescriptorSetLayout(devices,
                              &descriptor_pool);
    createGraphicsPipeline(VERTEX_SHADERS_PATH,
                           FRAGMENT_SHADERS_PATH,
                           devices,
                           swap_chain,
                           descriptor_pool,
                           graphics_pipeline,
                           msaa_samples.msaa_samples);
    create_command_pool(devices,
                        surface,
                        &command_pool);
    createDepthResources(devices,
                         queues,
                         msaa_samples,
                         &swap_chain,
                         &command_pool,
                         &depth_buffers);
    createColorResources();
    createFrameBuffers();
    create_texture_image(devices,
                         command_pool.command_pool,
                         queues.graphics_queue,
                         texture_model_resources.mip_levels,
                         texture_model_resources.texture_image,
                         texture_model_resources.texture_image_memory);
    createTextureImageView(devices,
                           texture_model_resources,
                           texture_model_resources_read_handler,
                           texture_model_resources.mip_levels);
    create_texture_sampler(devices,
                           texture_model_resources,
                           &texture_model_resources_read_handler);
    loadModel(&scene_model_resources);
    create_vertex_buffer(&devices,
                         &command_pool,
                         &queues,
                         &scene_model_resources);
    create_index_buffer(&devices,
                        &command_pool,
                        &queues,
                        &scene_model_resources);
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
    createCommandBuffers();
    createSyncObjects();

    std::cout << "Vulkan initialization process ends with success..." << std::endl;
  }

  void mainLoop() {
    std::cout << "Start main loop function..." << std::endl;
    while (!glfwWindowShouldClose(window.glfw_window)) {
      glfwPollEvents();
      drawFrame();
    }

    vkDeviceWaitIdle(devices.logical_device);

    std::cout << "Main loop function stoped..." << std::endl;
  }

  void cleanUp() {
    clean_up_swap_chain(devices,
                        color_image,
                        depth_buffers,
                        graphics_pipeline,
                        &swap_chain);

    vkDestroySampler(devices.logical_device, texture_model_resources_read_handler.texture_sampler, nullptr);
    vkDestroyImageView(devices.logical_device, texture_model_resources_read_handler.texture_image_view, nullptr);
    vkDestroyImage(devices.logical_device, texture_model_resources.texture_image, nullptr);
    vkFreeMemory(devices.logical_device, texture_model_resources.texture_image_memory, nullptr);

    for (size_t i = 0; i < command_pool.MAX_FRAMES_IN_FLIGHT; i++) {
      vkDestroyBuffer(devices.logical_device, uniform_buffers.uniform_buffers[i], nullptr);
      vkFreeMemory(devices.logical_device, uniform_buffers.uniform_buffers_memory[i], nullptr);
    }

    vkDestroyDescriptorPool(devices.logical_device, descriptor_pool.descriptor_pool, nullptr);
    vkDestroyDescriptorSetLayout(devices.logical_device, descriptor_pool.descriptor_set_layout, nullptr);
    vkDestroyBuffer(devices.logical_device, scene_model_resources.index_buffer, nullptr);
    vkFreeMemory(devices.logical_device, scene_model_resources.index_buffer_memory, nullptr);
    vkDestroyBuffer(devices.logical_device, scene_model_resources.vertex_buffer, nullptr);
    vkFreeMemory(devices.logical_device, scene_model_resources.vertex_buffer_memory, nullptr);

    for (size_t i = 0; i < command_pool.MAX_FRAMES_IN_FLIGHT; i++) {
      vkDestroySemaphore(devices.logical_device, renderFinishedSemaphores[i], nullptr);
      vkDestroySemaphore(devices.logical_device, imageAvailableSemaphores[i], nullptr);
      vkDestroyFence(devices.logical_device, inFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(devices.logical_device, command_pool.command_pool, nullptr);
    vkDestroyDevice(devices.logical_device, nullptr);

    if (enable_validation_layers) {
      destroy_debug_utils_messenger_EXT(instance.instance, debugMessenger, nullptr);
    }

    vkDestroySurfaceKHR(instance.instance, surface.surface_khr, nullptr);
    vkDestroyInstance(instance.instance, nullptr);
    glfwDestroyWindow(window.glfw_window);
    glfwTerminate();

    std::cout << "Clean up process ends with success..." << std::endl;
  }

  void recreateSwapChain() {
    int width = 0, height = 0;
    glfwGetFramebufferSize(window.glfw_window, &width, &height);
    while (width == 0 || height == 0) {
      glfwGetFramebufferSize(window.glfw_window, &width, &height);
      glfwWaitEvents();
    }

    vkDeviceWaitIdle(devices.logical_device);

    clean_up_swap_chain(devices,
                        color_image,
                        depth_buffers,
                        graphics_pipeline,
                        &swap_chain);

    create_swap_chain(devices,
                      surface,
                      window,
                      &swap_chain);
    create_image_views(devices, &swap_chain);
    create_render_pass(devices,
                       msaa_samples,
                       &graphics_pipeline,
                       &swap_chain);
    createGraphicsPipeline(VERTEX_SHADERS_PATH,
                           FRAGMENT_SHADERS_PATH,
                           devices,
                           swap_chain,
                           descriptor_pool,
                           graphics_pipeline,
                           msaa_samples.msaa_samples);
    createDepthResources(devices,
                         queues,
                         msaa_samples,
                         &swap_chain,
                         &command_pool,
                         &depth_buffers);
    createColorResources();
    createFrameBuffers();
    std::cout << "Swap chain recreation process ends with success..." << std::endl;
  }

  void createFrameBuffers() {
    swap_chain.swap_chain_frame_buffers.resize(swap_chain.swap_chain_image_views.size());
    for (size_t i = 0; i < swap_chain.swap_chain_image_views.size(); i++) {
      std::array<VkImageView, 3> attachments = {
          color_image.color_image_view,
          depth_buffers.depth_image_view,
          swap_chain.swap_chain_image_views[i],
      };

      VkFramebufferCreateInfo framebufferCreateInfo{};
      framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      framebufferCreateInfo.renderPass = graphics_pipeline.render_pass;
      framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
      framebufferCreateInfo.pAttachments = attachments.data();
      framebufferCreateInfo.width = swap_chain.swap_chain_extent.width;
      framebufferCreateInfo.height = swap_chain.swap_chain_extent.height;
      framebufferCreateInfo.layers = 1;

      if (VK_SUCCESS != vkCreateFramebuffer(devices.logical_device,
                                            &framebufferCreateInfo,
                                            nullptr,
                                            &swap_chain.swap_chain_frame_buffers[i])) {
        throw std::runtime_error("Failed to create framebuffer");
      }
    }

    std::cout << "Frame buffers creation and implementation processs ends with success..." << std::endl;
  }

  void createColorResources() {
    VkFormat colorFormat = swap_chain.swap_chain_image_format;

    create_image(swap_chain.swap_chain_extent.width, swap_chain.swap_chain_extent.height, 1, msaa_samples.msaa_samples,
                 colorFormat, VK_IMAGE_TILING_OPTIMAL,
                 VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, color_image.color_image, color_image.color_image_memory, devices);
    color_image.color_image_view =
        create_image_view(devices, color_image.color_image, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
  }

  void createUniformBuffers() {
    VkDeviceSize bufferSize = sizeof(SmUniformBufferObject);

    uniform_buffers.uniform_buffers.resize(command_pool.MAX_FRAMES_IN_FLIGHT);
    uniform_buffers.uniform_buffers_memory.resize(command_pool.MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < command_pool.MAX_FRAMES_IN_FLIGHT; i++) {
      create_buffer(&devices,
                    bufferSize,
                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                    &uniform_buffers.uniform_buffers[i],
                    &uniform_buffers.uniform_buffers_memory[i]);
    }

    std::cout << "Uniform buffer creation process ends with success..." << std::endl;
  }

  void copyBuffer(VkBuffer inputSrcBuffer, VkBuffer inputDstBuffer, VkDeviceSize inputBufferSize) {
    VkCommandBuffer commandBuffer = begin_single_time_commands(devices.logical_device, command_pool.command_pool);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;  // Optional
    copyRegion.dstOffset = 0;  // Optional
    copyRegion.size = inputBufferSize;
    vkCmdCopyBuffer(commandBuffer, inputSrcBuffer, inputDstBuffer, 1, &copyRegion);

    end_single_time_commands(devices.logical_device, command_pool.command_pool, queues.graphics_queue, commandBuffer);
  }

  void createDescriptorPool() {
    std::array<VkDescriptorPoolSize, 2> descriptorPoolSizes{};
    descriptorPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorPoolSizes[0].descriptorCount = static_cast<uint32_t>(command_pool.MAX_FRAMES_IN_FLIGHT);
    descriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorPoolSizes[1].descriptorCount = static_cast<uint32_t>(command_pool.MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
    descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();
    descriptorPoolCreateInfo.maxSets = static_cast<uint32_t>(command_pool.MAX_FRAMES_IN_FLIGHT);

    if (VK_SUCCESS != vkCreateDescriptorPool(devices.logical_device, &descriptorPoolCreateInfo, nullptr,
                                             &descriptor_pool.descriptor_pool)) {
      throw std::runtime_error("Failed to create descriptor pool for uniform buffers");
    }

    std::cout << "Description pool creation process ends with success..." << std::endl;
  }

  void createDescriptorSets() {
    std::vector<VkDescriptorSetLayout> layouts(command_pool.MAX_FRAMES_IN_FLIGHT,
                                               descriptor_pool.descriptor_set_layout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptor_pool.descriptor_pool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(command_pool.MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    descriptor_pool.descriptor_sets.resize(command_pool.MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(devices.logical_device, &allocInfo, descriptor_pool.descriptor_sets.data()) !=
        VK_SUCCESS) {
      throw std::runtime_error("Failed to allocate descriptor sets for uniform buffers");
    }

    for (size_t i = 0; i < command_pool.MAX_FRAMES_IN_FLIGHT; i++) {
      VkDescriptorBufferInfo bufferInfo{};
      bufferInfo.buffer = uniform_buffers.uniform_buffers[i];
      bufferInfo.offset = 0;
      bufferInfo.range = sizeof(SmUniformBufferObject);

      VkDescriptorImageInfo imageInfo{};
      imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      imageInfo.imageView = texture_model_resources_read_handler.texture_image_view;
      imageInfo.sampler = texture_model_resources_read_handler.texture_sampler;

      std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
      descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrites[0].dstSet = descriptor_pool.descriptor_sets[i];
      descriptorWrites[0].dstBinding = 0;
      descriptorWrites[0].dstArrayElement = 0;
      descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      descriptorWrites[0].descriptorCount = 1;
      descriptorWrites[0].pBufferInfo = &bufferInfo;

      descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrites[1].dstSet = descriptor_pool.descriptor_sets[i];
      descriptorWrites[1].dstBinding = 1;
      descriptorWrites[1].dstArrayElement = 0;
      descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      descriptorWrites[1].descriptorCount = 1;
      descriptorWrites[1].pImageInfo = &imageInfo;

      vkUpdateDescriptorSets(devices.logical_device, static_cast<uint32_t>(descriptorWrites.size()),
                             descriptorWrites.data(), 0, nullptr);
    }

    std::cout << "Descriptor sets creation process ends with success..." << std::endl;
  }

  void createCommandBuffers() {
    command_pool.command_buffers.resize(command_pool.MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = command_pool.command_pool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = (uint32_t) command_pool.command_buffers.size();

    if (VK_SUCCESS != vkAllocateCommandBuffers(devices.logical_device, &commandBufferAllocateInfo,
                                               command_pool.command_buffers.data())) {
      throw std::runtime_error("Failed to allocate command buffers");
    }

    std::cout << "Command buffers creation process ends with success..." << std::endl;
  }

  void createSyncObjects() {
    imageAvailableSemaphores.resize(command_pool.MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(command_pool.MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(command_pool.MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreCreateInfo{};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < command_pool.MAX_FRAMES_IN_FLIGHT; i++) {
      if (VK_SUCCESS !=
              vkCreateSemaphore(devices.logical_device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores[i]) ||
          VK_SUCCESS !=
              vkCreateSemaphore(devices.logical_device, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores[i]) ||
          VK_SUCCESS != vkCreateFence(devices.logical_device, &fenceCreateInfo, nullptr, &inFlightFences[i])) {
        throw std::runtime_error("Failed to create semaphores or fence");
      }
    }

    std::cout << "Sync objects creation process ends with success..." << std::endl;
  }

  int rateDeviceSuitability(VkPhysicalDevice input_device) {
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;

    vkGetPhysicalDeviceProperties(input_device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(input_device, &deviceFeatures);

    int score = 0;

    // Discrete GPUs have a significant performance advantage
    if (VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU == deviceProperties.deviceType) {
      score += 1000;
    }

    // Maximum possible size of textures affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D;

    // Application can't function without geometry shaders
    if (!deviceFeatures.geometryShader) {
      return 0;
    }

    return score;
  }

  void drawFrame() {
    // Create the fence in the signaled state, so that the first call to vkWaitForFences() returns immediately since
    // the fence is already signaled. This builted into the API.
    // This behavior reached by flag = VK_FENCE_CREATE_SIGNALED_BIT.
    vkWaitForFences(devices.logical_device,
                    1,
                    &inFlightFences[currentFrame],
                    VK_TRUE,
                    UINT64_MAX);

    uint32_t imageIndex;
    VkResult acquireImageResult =
        vkAcquireNextImageKHR(devices.logical_device,
                              swap_chain.swap_chain,
                              UINT64_MAX,
                              imageAvailableSemaphores[currentFrame],
                              VK_NULL_HANDLE,
                              &imageIndex);
    if (VK_ERROR_OUT_OF_DATE_KHR == acquireImageResult) {
      recreateSwapChain();
      return;
    } else if (VK_SUCCESS != acquireImageResult && VK_SUBOPTIMAL_KHR != acquireImageResult) {
      throw std::runtime_error("Failed to acquired swap chain image");
    }

    vkResetFences(devices.logical_device,
                  1,
                  &inFlightFences[currentFrame]);  // Only reset the fence if we are submitting work

    vkResetCommandBuffer(command_pool.command_buffers[currentFrame], 0);
    recordCommandBuffer(command_pool.command_buffers[currentFrame], imageIndex);

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    updateUniformBuffer(currentFrame);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &command_pool.command_buffers[currentFrame];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (VK_SUCCESS != vkQueueSubmit(queues.graphics_queue,
                                    1,
                                    &submitInfo,
                                    inFlightFences[currentFrame])) {
      throw std::runtime_error("Failed to submit draw command buffer");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swap_chain.swap_chain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    VkResult queuePresentResult = vkQueuePresentKHR(queues.present_queue, &presentInfo);
    if (VK_ERROR_OUT_OF_DATE_KHR == queuePresentResult ||
        VK_SUBOPTIMAL_KHR == queuePresentResult ||
        framebufferResized) {
      framebufferResized = false;
      recreateSwapChain();
    } else if (VK_SUCCESS != queuePresentResult) {
      throw std::runtime_error("Failed to preset swap chain image");
    }

    currentFrame = (currentFrame + 1) % command_pool.MAX_FRAMES_IN_FLIGHT;
  }

  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo bufferBeginInfo{};
    bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    bufferBeginInfo.flags = 0;                   // Optional
    bufferBeginInfo.pInheritanceInfo = nullptr;  // optional

    if (VK_SUCCESS != vkBeginCommandBuffer(commandBuffer, &bufferBeginInfo)) {
      throw std::runtime_error("Failed to begin recording command buffer");
    }

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = graphics_pipeline.render_pass;
    renderPassBeginInfo.framebuffer = swap_chain.swap_chain_frame_buffers[imageIndex];
    renderPassBeginInfo.renderArea.offset = {0, 0};
    renderPassBeginInfo.renderArea.extent = swap_chain.swap_chain_extent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline.pipeline);

    VkBuffer vertexBuffers[] = {scene_model_resources.vertex_buffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, scene_model_resources.index_buffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(commandBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            graphics_pipeline.pipeline_layout,
                            0,
                            1,
                            descriptor_pool.descriptor_sets.data(),
                            0,
                            nullptr);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(scene_model_resources.indices.size()),
                     1,
                     0,
                     0,
                     0);

    vkCmdEndRenderPass(commandBuffer);

    if (VK_SUCCESS != vkEndCommandBuffer(commandBuffer)) {
      throw std::runtime_error("Failed to record command buffer");
    }

    // Documentation's description for vkCmdDraw function. Parameters:
    // vertexCount - Even though we don't have a vertex buffer, we technically still have 3
    // scene_model_resources.vertices to draw. instanceCount - Used for instanced rendering, use 1 if you're not doing
    // that. firstVertex - Used as an offset into the vertex buffer, defines the lowest value of gl_VertexIndex.
    // firstInstance - Used as an offset for instanced rendering, defines the lowest value of gl_InstanceIndex.
  }

  void updateUniformBuffer(uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    SmUniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f),
                            time * glm::radians(20.0f),
                            glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
                           glm::vec3(0.0f, 0.0f, 0.0f),
                           glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj =
        glm::perspective(glm::radians(45.0f),
                         swap_chain.swap_chain_extent.width / (float) swap_chain.swap_chain_extent.height,
                         0.1f,
                         10.0f);
    ubo.proj[1][1] *= -1;

    // Bug reason is:
    void* data;
    vkMapMemory(devices.logical_device, uniform_buffers.uniform_buffers_memory.at(currentImage), 0,
                (VkDeviceSize) sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(devices.logical_device, uniform_buffers.uniform_buffers_memory.at(currentImage));
  }

 public:
  void run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanUp();
  }
};

int main() {
  SmVulkanRendererSystem app;

  try {
    app.run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
