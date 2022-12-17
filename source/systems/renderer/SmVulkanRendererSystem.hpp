/*
------------------------------------
  Slimemaid Source Code (18.05.2022)
  This file is part of Slimemaid Source Code.
------------------------------------
*/

#ifndef SLIMEMAID_SMVULKANRENDERER_HPP
#define SLIMEMAID_SMVULKANRENDERER_HPP

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORSE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <algorithm>    // Necessary for std::clamp
#include <array>
#include <cassert>
#include <chrono>
#include <cstdint>      // Necessary for uint32_t
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>       // Necessary for std::numeric_limits
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
#include "components/renderer/SmGraphicsPipeline.hpp"
#include "components/renderer/SmModelResources.hpp"
#include "components/renderer/SmQueues.hpp"
#include "components/renderer/SmSamplingFlags.hpp"
#include "components/renderer/SmSurface.hpp"
#include "components/renderer/SmSwapChain.hpp"
#include "components/renderer/SmTextureImage.hpp"
#include "components/renderer/SmTextureImageViewSampler.hpp"
#include "components/renderer/SmUniformBufferObject.hpp"
#include "components/renderer/SmUniformBuffers.hpp"
#include "components/renderer/SmVulkanInstance.hpp"

#include "systems/renderer/SmBuffersSystem.hpp"
#include "systems/renderer/SmCommandsSystem.hpp"
#include "systems/renderer/SmDepthBuffersSystem.hpp"
#include "systems/renderer/SmDescriptorsSystem.hpp"
#include "systems/renderer/SmDeviceSystem.hpp"
#include "systems/renderer/SmFrameBufferSystem.hpp"
#include "systems/renderer/SmGLFWWindowSystem.hpp"
#include "systems/renderer/SmGraphicsMemorySystem.hpp"
#include "systems/renderer/SmGraphicsPipelineSystem.hpp"
#include "systems/renderer/SmImageViewSystem.hpp"
#include "systems/renderer/SmModelLoaderSystem.hpp"
#include "systems/renderer/SmQueueFamiliesSystem.hpp"
#include "systems/renderer/SmSamplingFlagsSystem.hpp"
#include "systems/renderer/SmShaderSystem.hpp"
#include "systems/renderer/SmSwapChainSystem.hpp"
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

  std::vector<VkSemaphore> image_available_semaphores;
  std::vector<VkSemaphore> render_finished_semaphores;
  std::vector<VkFence> in_flight_fences;

  bool frame_buffer_resized_flag = false;
  uint32_t current_frame_index = 0;

  void init_window() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window.glfw_window = glfwCreateWindow((int) frameParams.WIDTH,
                                          (int) frameParams.HEIGHT,
                                          "Vulkan",
                                          nullptr,
                                          nullptr);
    glfwSetWindowUserPointer(window.glfw_window, this);
    glfwSetFramebufferSizeCallback(window.glfw_window, frame_buffer_resize_callback);
    std::cout << "GLFW initialization process ends with success..." << std::endl;
  }

  static void frame_buffer_resize_callback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<SmVulkanRendererSystem*>(glfwGetWindowUserPointer(window));
    app->frame_buffer_resized_flag = true;
  }

  void init_vulkan() {
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
                       &swap_chain,
                       &graphics_pipeline);
    create_descriptor_set_layout(devices, &descriptor_pool);
    create_graphics_pipeline(VERTEX_SHADERS_PATH,
                             FRAGMENT_SHADERS_PATH,
                             devices,
                             &swap_chain,
                             &descriptor_pool,
                             &graphics_pipeline,
                             msaa_samples.msaa_samples);
    create_command_pool(devices,
                        surface,
                        &command_pool);
    create_depth_resources(devices, queues, msaa_samples, &swap_chain, &command_pool, &depth_buffers);
    create_color_resources(devices,
                           swap_chain,
                           msaa_samples,
                           &color_image);
    create_frame_buffers(devices,
                         graphics_pipeline,
                         color_image,
                         depth_buffers,
                         &swap_chain);
    create_texture_image(devices,
                         command_pool.command_pool,
                         queues.graphics_queue,
                         texture_model_resources.mip_levels,
                         texture_model_resources.texture_image,
                         texture_model_resources.texture_image_memory);
    create_texture_image_view(devices,
                              texture_model_resources,
                              &texture_model_resources_read_handler,
                              texture_model_resources.mip_levels);
    create_texture_sampler(devices,
                           texture_model_resources,
                           &texture_model_resources_read_handler);
    load_model(&scene_model_resources);
    create_vertex_buffer(devices,
                         &command_pool,
                         &queues,
                         &scene_model_resources);
    create_index_buffer(devices,
                        &command_pool,
                        &queues,
                        &scene_model_resources);
    create_uniform_buffers(devices,
                           &command_pool,
                           &uniform_buffers);
    create_descriptor_pool(devices,
                           &command_pool,
                           &descriptor_pool);
    create_descriptor_sets(devices,
                           texture_model_resources_read_handler,
                           &uniform_buffers,
                           &command_pool,
                           &descriptor_pool);
    create_command_buffers(devices,
                           &command_pool);
    create_sync_objects(devices, &command_pool, &image_available_semaphores, &render_finished_semaphores,
                        &in_flight_fences);

    std::cout << "Vulkan initialization process ends with success..." << std::endl;
  }

  void main_loop() {
    std::cout << "Start main loop function..." << std::endl;
    while (!glfwWindowShouldClose(window.glfw_window)) {
      glfwPollEvents();
      draw_frame();
    }

    vkDeviceWaitIdle(devices.logical_device);

    std::cout << "Main loop function stoped..." << std::endl;
  }

  void clean_up() {
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
      vkDestroySemaphore(devices.logical_device, render_finished_semaphores[i], nullptr);
      vkDestroySemaphore(devices.logical_device, image_available_semaphores[i], nullptr);
      vkDestroyFence(devices.logical_device, in_flight_fences[i], nullptr);
    }

    vkDestroyCommandPool(devices.logical_device, command_pool.command_pool, nullptr);
    vkDestroyDevice(devices.logical_device, nullptr);

    if (enable_validation_layers) {
      destroy_debug_utils_messenger_EXT(instance.instance, &debugMessenger, nullptr);
    }

    vkDestroySurfaceKHR(instance.instance, surface.surface_khr, nullptr);
    vkDestroyInstance(instance.instance, nullptr);
    glfwDestroyWindow(window.glfw_window);
    glfwTerminate();

    std::cout << "Clean up process ends with success..." << std::endl;
  }

  void recreate_swap_chain() {
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
                       &swap_chain,
                       &graphics_pipeline);
    create_graphics_pipeline(VERTEX_SHADERS_PATH,
                             FRAGMENT_SHADERS_PATH,
                             devices,
                             &swap_chain,
                             &descriptor_pool,
                             &graphics_pipeline,
                             msaa_samples.msaa_samples);
    create_depth_resources(devices, queues, msaa_samples, &swap_chain, &command_pool, &depth_buffers);
    create_color_resources(devices,
                           swap_chain,
                           msaa_samples,
                           &color_image);
    create_frame_buffers(devices,
                         graphics_pipeline,
                         color_image,
                         depth_buffers,
                         &swap_chain);
    std::cout << "Swap chain recreation process ends with success..." << std::endl;
  }

  void draw_frame() {
    // Create the fence in the signaled state, so that the first call to vkWaitForFences() returns immediately since
    // the fence is already signaled. This builted into the API.
    // This behavior reached by flag = VK_FENCE_CREATE_SIGNALED_BIT.
    vkWaitForFences(devices.logical_device,
                    1,
                    &in_flight_fences[current_frame_index],
                    VK_TRUE,
                    UINT64_MAX);

    uint32_t image_index;
    VkResult acquireImageResult = vkAcquireNextImageKHR(devices.logical_device,
                                                        swap_chain.swap_chain,
                                                        UINT64_MAX,
                                                        image_available_semaphores[current_frame_index],
                                                        VK_NULL_HANDLE,
                                                        &image_index);

    if (VK_ERROR_OUT_OF_DATE_KHR == acquireImageResult) {
      recreate_swap_chain();
      return;
    } else if (VK_SUCCESS != acquireImageResult && VK_SUBOPTIMAL_KHR != acquireImageResult) {
      throw std::runtime_error("Failed to acquired swap chain image");
    }

    vkResetFences(devices.logical_device,
                  1,
                  &in_flight_fences[current_frame_index]);  // Only reset the fence if we are submitting work

    vkResetCommandBuffer(command_pool.command_buffers[current_frame_index],
                         0);
    record_command_buffer(image_index,
                          current_frame_index,
                          command_pool,
                          graphics_pipeline,
                          &swap_chain,
                          &descriptor_pool,
                          &scene_model_resources);

    VkSemaphore semaphores_waitresses[] = {image_available_semaphores[current_frame_index]};
    VkSemaphore semaphores_signals[] = {render_finished_semaphores[current_frame_index]};
    VkPipelineStageFlags stages_waitresses[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    update_uniform_buffer_state(devices,
                                &swap_chain,
                                &uniform_buffers,
                                current_frame_index);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = semaphores_waitresses;
    submitInfo.pWaitDstStageMask = stages_waitresses;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &command_pool.command_buffers[current_frame_index];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = semaphores_signals;

    if (VK_SUCCESS != vkQueueSubmit(queues.graphics_queue,
                                    1,
                                    &submitInfo, in_flight_fences[current_frame_index])) {
      throw std::runtime_error("Failed to submit draw command buffer");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = semaphores_signals;

    VkSwapchainKHR swapChains[] = {swap_chain.swap_chain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &image_index;

    VkResult queuePresentResult = vkQueuePresentKHR(queues.present_queue, &presentInfo);
    if (VK_ERROR_OUT_OF_DATE_KHR == queuePresentResult ||
        VK_SUBOPTIMAL_KHR == queuePresentResult ||
        frame_buffer_resized_flag) {
      frame_buffer_resized_flag = false;
      recreate_swap_chain();
    } else if (VK_SUCCESS != queuePresentResult) {
      throw std::runtime_error("Failed to preset swap chain image");
    }

    current_frame_index = (current_frame_index + 1) % command_pool.MAX_FRAMES_IN_FLIGHT;
  }

  static void update_uniform_buffer_state(SmDevices input_devices,
                                          SmSwapChain* p_swap_chain,
                                          SmUniformBuffers* p_uniform_buffers,
                                          uint32_t input_current_image) {
    static auto start_time = std::chrono::high_resolution_clock::now();

    auto current_time = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();

    SmUniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f),
                            time * glm::radians(20.0f),
                            glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
                           glm::vec3(0.0f, 0.0f, 0.0f),
                           glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f),
                                (float) p_swap_chain->swap_chain_extent.width /
                                    (float) p_swap_chain->swap_chain_extent.height,
                                0.1f,
                                10.0f);
    ubo.proj[1][1] *= -1;

    void* data;
    vkMapMemory(input_devices.logical_device,
                p_uniform_buffers->uniform_buffers_memory.at(input_current_image),
                0,
                (VkDeviceSize) sizeof(ubo),
                0,
                &data);
    memcpy(data,
           &ubo,
           sizeof(ubo));
    vkUnmapMemory(input_devices.logical_device,
                  p_uniform_buffers->uniform_buffers_memory.at(input_current_image));
  }

 public:
  void run() {
    init_window();
    init_vulkan();
    main_loop();
    clean_up();
  }
};

#endif  // SLIMEMAID_SMVULKANRENDERER_HPP
