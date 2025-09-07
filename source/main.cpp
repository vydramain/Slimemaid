#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORSE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <array>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
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
#include "components/renderer/SmQueues.hpp"
#include "components/renderer/SmSamplingFlags.hpp"
#include "components/renderer/SmSurface.hpp"
#include "components/renderer/SmSwapChain.hpp"
#include "components/renderer/SmTextureImage.hpp"
#include "components/renderer/SmTextureImageViewSampler.hpp"
#include "components/renderer/SmUniformBufferObject.hpp"
#include "components/renderer/SmUniformBuffers.hpp"
#include "components/renderer/SmVulkanInstance.hpp"
#include "systems/debug/SmDebugSystem.hpp"
#include "systems/renderer/SmBuffersSystem.hpp"
#include "systems/renderer/SmCommandsSystem.hpp"
#include "systems/renderer/SmDepthBuffersSystem.hpp"
#include "systems/renderer/SmDescriptorsSystem.hpp"
#include "systems/renderer/SmDeviceSystem.hpp"
#include "systems/renderer/SmFrameBufferSystem.hpp"
#include "systems/renderer/SmGLFWWindowSystem.hpp"
#include "systems/renderer/SmGraphicsPipelineSystem.hpp"
#include "systems/renderer/SmImageViewSystem.hpp"
#include "systems/renderer/SmModelLoaderSystem.hpp"
#include "systems/renderer/SmSwapChainSystem.hpp"
#include "systems/renderer/SmTextureImageSystem.hpp"
#include "systems/renderer/SmVulkanInstanceSystem.hpp"

const std::string VERTEX_SHADERS_PATH = "./shaders/vert.spv";
const std::string FRAGMENT_SHADERS_PATH = "./shaders/frag.spv";

class SmVulkanRendererSystem {
 private:
  SmFrame frame_params;
  SmGLFWWindow window{};
  SmSurface surface{};
  SmVulkanInstance instance{};

  VkDebugUtilsMessengerEXT debug_messenger{};

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

  const bool enable_validation_layers = true;

  std::vector<const char*> validation_layers = {
      "VK_LAYER_KHRONOS_validation",
      "VK_LAYER_LUNARG_monitor"    // Display the Vulkan application FPS in the
                                   // window title bar to give a hint about the
                                   // performance
  };
  std::vector<const char*> device_extensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  void sl_init_window() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window.glfw_window =
        glfwCreateWindow((int)frame_params.WIDTH, (int)frame_params.HEIGHT,
                         "Vulkan", nullptr, nullptr);
    glfwSetWindowUserPointer(window.glfw_window, this);
    glfwSetFramebufferSizeCallback(window.glfw_window,
                                   sl_framebuffer_resize_callback);
    std::cout << "GLFW initialization process ends with success..."
              << std::endl;
  }

  static void sl_framebuffer_resize_callback(GLFWwindow* window, int width,
                                             int height) {
    auto app = reinterpret_cast<SmVulkanRendererSystem*>(
        glfwGetWindowUserPointer(window));
    app->frame_buffer_resized_flag = true;
  }

  void sl_init_vulkan() {
    std::cout << "Start initialization of Vulkan instance..." << std::endl;

    sl_create_instance(&instance, enable_validation_layers, &validation_layers);
    sl_setup_debug_messenger(enable_validation_layers, instance,
                             &debug_messenger);
    sl_create_surface(window, instance, &surface);
    sl_pick_physical_device(instance, &msaa_samples, &devices, surface,
                            device_extensions);
    sl_create_logical_device(&devices, surface, &queues,
                             enable_validation_layers, validation_layers,
                             device_extensions);
    sl_create_swap_chain(devices, surface, window, &swap_chain);
    sl_create_image_views(devices, &swap_chain);
    sl_create_render_pass(devices, msaa_samples, &graphics_pipeline,
                          &swap_chain);
    sl_create_descriptor_set_layout(devices, &descriptor_pool);
    sl_create_graphics_pipeline(VERTEX_SHADERS_PATH, FRAGMENT_SHADERS_PATH,
                                devices, swap_chain, descriptor_pool,
                                graphics_pipeline, msaa_samples.msaa_samples);
    sl_create_command_pool(devices, surface, &command_pool);
    sl_create_depth_resources(devices, queues, msaa_samples, &swap_chain,
                              &command_pool, &depth_buffers);
    sl_create_color_resources(devices, swap_chain, msaa_samples, &color_image);
    sl_create_frame_buffers(devices, graphics_pipeline, color_image,
                            depth_buffers, &swap_chain);
    sl_create_texture_image(devices, command_pool.command_pool,
                            queues.graphics_queue,
                            texture_model_resources.mip_levels,
                            texture_model_resources.texture_image,
                            texture_model_resources.texture_image_memory);
    sl_create_texture_image_view(devices, texture_model_resources,
                                 texture_model_resources_read_handler,
                                 texture_model_resources.mip_levels);
    sl_create_texture_sampler(devices, texture_model_resources,
                              &texture_model_resources_read_handler);
    sl_load_model(&scene_model_resources);
    sl_create_vertex_buffer(devices, &command_pool, &queues,
                            &scene_model_resources);
    sl_create_index_buffer(devices, &command_pool, &queues,
                           &scene_model_resources);
    sl_create_uniform_buffers(devices, &command_pool, &uniform_buffers);
    sl_create_descriptor_pool(devices, &command_pool, &descriptor_pool);
    sl_create_descriptor_sets(devices, texture_model_resources_read_handler,
                              &uniform_buffers, &command_pool,
                              &descriptor_pool);
    sl_create_command_buffers(devices, &command_pool);
    sl_create_sync_objects(devices, &command_pool, &image_available_semaphores,
                           &render_finished_semaphores, &in_flight_fences);

    std::cout << "Vulkan initialization process ends with success..."
              << std::endl;
  }

  void sl_main_loop() {
    std::cout << "Start main loop function..." << std::endl;
    while (!glfwWindowShouldClose(window.glfw_window)) {
      glfwPollEvents();
      sl_draw_frame();
    }

    vkDeviceWaitIdle(devices.logical_device);

    std::cout << "Main loop function stopped..." << std::endl;
  }

  void sl_clean_up() {
    sl_clean_up_swap_chain(devices, color_image, depth_buffers,
                           graphics_pipeline, &swap_chain);

    vkDestroySampler(devices.logical_device,
                     texture_model_resources_read_handler.texture_sampler,
                     nullptr);
    vkDestroyImageView(devices.logical_device,
                       texture_model_resources_read_handler.texture_image_view,
                       nullptr);
    vkDestroyImage(devices.logical_device,
                   texture_model_resources.texture_image, nullptr);
    vkFreeMemory(devices.logical_device,
                 texture_model_resources.texture_image_memory, nullptr);

    for (size_t i = 0; i < command_pool.MAX_FRAMES_IN_FLIGHT; i++) {
      vkDestroyBuffer(devices.logical_device,
                      uniform_buffers.uniform_buffers[i], nullptr);
      vkFreeMemory(devices.logical_device,
                   uniform_buffers.uniform_buffers_memory[i], nullptr);
    }

    vkDestroyDescriptorPool(devices.logical_device,
                            descriptor_pool.descriptor_pool, nullptr);
    vkDestroyDescriptorSetLayout(
        devices.logical_device, descriptor_pool.descriptor_set_layout, nullptr);
    vkDestroyBuffer(devices.logical_device, scene_model_resources.index_buffer,
                    nullptr);
    vkFreeMemory(devices.logical_device,
                 scene_model_resources.index_buffer_memory, nullptr);
    vkDestroyBuffer(devices.logical_device, scene_model_resources.vertex_buffer,
                    nullptr);
    vkFreeMemory(devices.logical_device,
                 scene_model_resources.vertex_buffer_memory, nullptr);

    for (size_t i = 0; i < command_pool.MAX_FRAMES_IN_FLIGHT; i++) {
      vkDestroySemaphore(devices.logical_device, render_finished_semaphores[i],
                         nullptr);
      vkDestroySemaphore(devices.logical_device, image_available_semaphores[i],
                         nullptr);
      vkDestroyFence(devices.logical_device, in_flight_fences[i], nullptr);
    }

    vkDestroyCommandPool(devices.logical_device, command_pool.command_pool,
                         nullptr);
    vkDestroyDevice(devices.logical_device, nullptr);

    if (enable_validation_layers) {
      sl_destroy_debug_utils_messenger_EXT(instance.instance, debug_messenger,
                                           nullptr);
    }

    vkDestroySurfaceKHR(instance.instance, surface.surface_khr, nullptr);
    vkDestroyInstance(instance.instance, nullptr);
    glfwDestroyWindow(window.glfw_window);
    glfwTerminate();

    std::cout << "Clean up process ends with success..." << std::endl;
  }

  void sl_recreate_swap_chain() {
    int width = 0, height = 0;
    glfwGetFramebufferSize(window.glfw_window, &width, &height);
    while (width == 0 || height == 0) {
      glfwGetFramebufferSize(window.glfw_window, &width, &height);
      glfwWaitEvents();
    }

    vkDeviceWaitIdle(devices.logical_device);

    sl_clean_up_swap_chain(devices, color_image, depth_buffers,
                           graphics_pipeline, &swap_chain);

    sl_create_swap_chain(devices, surface, window, &swap_chain);
    sl_create_image_views(devices, &swap_chain);
    sl_create_render_pass(devices, msaa_samples, &graphics_pipeline,
                          &swap_chain);
    sl_create_graphics_pipeline(VERTEX_SHADERS_PATH, FRAGMENT_SHADERS_PATH,
                                devices, swap_chain, descriptor_pool,
                                graphics_pipeline, msaa_samples.msaa_samples);
    sl_create_depth_resources(devices, queues, msaa_samples, &swap_chain,
                              &command_pool, &depth_buffers);
    sl_create_color_resources(devices, swap_chain, msaa_samples, &color_image);
    sl_create_frame_buffers(devices, graphics_pipeline, color_image,
                            depth_buffers, &swap_chain);
    std::cout << "Swap chain recreation process ends with success..."
              << std::endl;
  }

  int sl_rate_device_suitability(VkPhysicalDevice input_device) {
    VkPhysicalDeviceProperties device_properties;
    VkPhysicalDeviceFeatures device_features;

    vkGetPhysicalDeviceProperties(input_device, &device_properties);
    vkGetPhysicalDeviceFeatures(input_device, &device_features);

    int score = 0;

    if (VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU == device_properties.deviceType) {
      score += 1000;
    }

    score += device_properties.limits.maxImageDimension2D;

    if (!device_features.geometryShader) {
      return 0;
    }

    return score;
  }

  void sl_draw_frame() {
    vkWaitForFences(devices.logical_device, 1,
                    &in_flight_fences[current_frame_index], VK_TRUE,
                    UINT64_MAX);

    uint32_t image_index;
    VkResult acquire_image_result = vkAcquireNextImageKHR(
        devices.logical_device, swap_chain.swap_chain, UINT64_MAX,
        image_available_semaphores[current_frame_index], VK_NULL_HANDLE,
        &image_index);

    if (VK_ERROR_OUT_OF_DATE_KHR == acquire_image_result) {
      sl_recreate_swap_chain();
      return;
    } else if (VK_SUCCESS != acquire_image_result &&
               VK_SUBOPTIMAL_KHR != acquire_image_result) {
      throw std::runtime_error("Failed to acquired swap chain image");
    }

    vkResetFences(devices.logical_device, 1,
                  &in_flight_fences[current_frame_index]);

    vkResetCommandBuffer(command_pool.command_buffers[current_frame_index], 0);
    sl_record_command_buffer(command_pool.command_buffers[current_frame_index],
                             image_index);

    VkSemaphore wait_semaphores[] = {
        image_available_semaphores[current_frame_index]};
    VkSemaphore signal_semaphores[] = {
        render_finished_semaphores[current_frame_index]};
    VkPipelineStageFlags wait_stages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    sl_update_uniform_buffer(current_frame_index);

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers =
        &command_pool.command_buffers[current_frame_index];
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    if (VK_SUCCESS != vkQueueSubmit(queues.graphics_queue, 1, &submit_info,
                                    in_flight_fences[current_frame_index])) {
      throw std::runtime_error("Failed to submit draw command buffer");
    }

    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;

    VkSwapchainKHR swap_chains[] = {swap_chain.swap_chain};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swap_chains;

    present_info.pImageIndices = &image_index;

    VkResult queue_present_result =
        vkQueuePresentKHR(queues.present_queue, &present_info);
    if (VK_ERROR_OUT_OF_DATE_KHR == queue_present_result ||
        VK_SUBOPTIMAL_KHR == queue_present_result ||
        frame_buffer_resized_flag) {
      frame_buffer_resized_flag = false;
      sl_recreate_swap_chain();
    } else if (VK_SUCCESS != queue_present_result) {
      throw std::runtime_error("Failed to preset swap chain image");
    }

    current_frame_index =
        (current_frame_index + 1) % command_pool.MAX_FRAMES_IN_FLIGHT;
  }

  void sl_record_command_buffer(VkCommandBuffer command_buffer,
                                uint32_t image_index) {
    VkCommandBufferBeginInfo buffer_begin_info{};
    buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    buffer_begin_info.flags = 0;
    buffer_begin_info.pInheritanceInfo = nullptr;

    if (VK_SUCCESS !=
        vkBeginCommandBuffer(command_buffer, &buffer_begin_info)) {
      throw std::runtime_error("Failed to begin recording command buffer");
    }

    VkRenderPassBeginInfo render_pass_begin_info{};
    render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_begin_info.renderPass = graphics_pipeline.render_pass;
    render_pass_begin_info.framebuffer =
        swap_chain.swap_chain_frame_buffers[image_index];
    render_pass_begin_info.renderArea.offset = {0, 0};
    render_pass_begin_info.renderArea.extent = swap_chain.swap_chain_extent;

    std::array<VkClearValue, 2> clear_values{};
    clear_values[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clear_values[1].depthStencil = {1.0f, 0};

    render_pass_begin_info.clearValueCount =
        static_cast<uint32_t>(clear_values.size());
    render_pass_begin_info.pClearValues = clear_values.data();

    vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info,
                         VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      graphics_pipeline.pipeline);

    VkBuffer vertex_buffers[] = {scene_model_resources.vertex_buffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);
    vkCmdBindIndexBuffer(command_buffer, scene_model_resources.index_buffer, 0,
                         VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            graphics_pipeline.pipeline_layout, 0, 1,
                            descriptor_pool.descriptor_sets.data(), 0, nullptr);
    vkCmdDrawIndexed(
        command_buffer,
        static_cast<uint32_t>(scene_model_resources.indices.size()), 1, 0, 0,
        0);

    vkCmdEndRenderPass(command_buffer);

    if (VK_SUCCESS != vkEndCommandBuffer(command_buffer)) {
      throw std::runtime_error("Failed to record command buffer");
    }
  }

  void sl_update_uniform_buffer(uint32_t current_image) {
    static auto start_time = std::chrono::high_resolution_clock::now();

    auto current_time = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(
                     current_time - start_time)
                     .count();

    SmUniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(20.0f),
                            glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view =
        glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f),
                                swap_chain.swap_chain_extent.width /
                                    (float)swap_chain.swap_chain_extent.height,
                                0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    void* data;
    vkMapMemory(devices.logical_device,
                uniform_buffers.uniform_buffers_memory.at(current_image), 0,
                (VkDeviceSize)sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(devices.logical_device,
                  uniform_buffers.uniform_buffers_memory.at(current_image));
  }

 public:
  void sl_run() {
    sl_init_window();
    sl_init_vulkan();
    sl_main_loop();
    sl_clean_up();
  }
};

int main() {
  SmVulkanRendererSystem app;

  try {
    app.sl_run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
