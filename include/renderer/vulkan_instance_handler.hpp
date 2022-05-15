/*
------------------------------------

Slimemaid Source Code (01.05.2022)

This file is part of Slimemaid Source Code.

This file has heading of allocation functions for Vulkan API.

------------------------------------
*/

#ifndef SM_VULKAN_INSTANCE_
#define SM_VULKAN_INSTANCE_

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORSE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES  // dif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <algorithm>  // Necessary for std::clamp
#include <array>
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

#include "allocator.hpp"
#include "consts.hpp"
#include "debug_messenger.hpp"
#include "glfw_handler.hpp"
#include "memory_handler.hpp"
#include "structs/queue_family_indices.hpp"
#include "structs/swap_chain_support_details.hpp"
#include "structs/uniform_buffer_object.hpp"
#include "structs/vertex.hpp"

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

const std::string MODEL_PATH = "raws/viking_room/viking_room.obj";
const std::string TEXTURE_PATH = "raws/viking_room/viking_room.png";

class vulkan_instance_handler {
 private:
  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;
  VkSurfaceKHR surface;

  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device;

  VkQueue graphicsQueue;
  VkQueue presentQueue;

  VkSwapchainKHR swapChain;
  std::vector<VkImage> swapChainImages;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;
  std::vector<VkImageView> swapChainImageViews;
  std::vector<VkFramebuffer> swapChainFramebuffers;

  VkRenderPass renderPass;
  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;

  VkDescriptorPool descriptorPool;
  VkDescriptorSetLayout descriptorSetLayout;
  std::vector<VkDescriptorSet> descriptorSets;

  VkCommandPool commandPool;

  VkImage depthImage;
  VkDeviceMemory depthImageMemory;
  VkImageView depthImageView;
  VkImageView textureImageView;

  uint32_t mipLevels;
  VkImage textureImage;
  VkDeviceMemory textureImageMemory;
  VkSampler textureSampler;

  std::vector<Vertex> vertices;  // {{rel_x, rel_y, rel_z}, {R, G, B}, {tex_rel_x, tex_rel_x}}
  std::vector<uint32_t> indices;

  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  VkBuffer indexBuffer;
  VkDeviceMemory indexBufferMemory;

  std::vector<VkBuffer> uniformBuffers;
  std::vector<VkDeviceMemory> uniformBuffersMemory;

  std::vector<VkCommandBuffer> commandBuffers;

  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;

  VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

  VkImage colorImage;
  VkDeviceMemory colorImageMemory;
  VkImageView colorImageView;

  uint32_t currentFrame = 0;

  void initWindow();
  void initVulkan();
  void mainLoop();
  void cleanUp();

  void cleanUpSwapChain();
  void recreateSwapChain();

  void createInstance();
  void setupDebugMessenger();
  void createSurface();
  void pickPhysicalDevice();
  void createLogicalDevice();
  void createSwapChain();
  void createImageViews();
  void createRenderPass();
  void createDescriptorSetLayout();
  void createGraphicsPipeline();
  void createCommandPool();
  void createDepthResources();
  void createColorResources();
  void createFramebuffers();
  void createTextureImage();
  void createTextureImageView();
  void createTextureSampler();
  void loadModel();
  void createVertexBuffer();
  void createIndexBuffer();
  void createUniformBuffers();
  void createDescriptorPool();
  void createDescriptorSets();
  void createCommandBuffers();
  void createSyncObjects();

  VkFormat findSupportedDepthFormat(const std::vector<VkFormat> &candidates, VkImageTiling inputTiling,
                                    VkFormatFeatureFlags inputFlags);
  void transitionImageLayout(VkImage inputImage, VkFormat inputFormat, VkImageLayout inputOldImageLayout,
                             VkImageLayout inputNewImageLayout, uint32_t inputMipLevels);
  void generateMipmaps(VkImage inputImage, VkFormat inputImageFormat, uint32_t inputTextureWidth,
                       uint32_t inputTextureHeight, uint32_t inputMipLevels);
  VkImageView createImageView(VkImage inputImage, VkFormat inputFormat, VkImageAspectFlags inputAspectMask,
                              uint32_t inputMipLevels);
  void copyBufferToImage(VkBuffer inputBuffer, VkImage inputImage, uint32_t inputWidth, uint32_t inputHeight);
  void createBuffer(VkDeviceSize inputSize, VkBufferUsageFlags inputUsage, VkMemoryPropertyFlags inputMemoryProperties,
                    VkBuffer &buffer, VkDeviceMemory &bufferMemory);

  void drawFrame();
  VkFormat findDepthFormat();
  VkSampleCountFlagBits getMaxUsableSampleCount();
  bool checkValidationLayerSupport();
  bool hasStencilComponent(VkFormat format);
  VkCommandBuffer beginSingleTimeCommands();
  bool isDeviceSuitable(VkPhysicalDevice device);
  void updateUniformBuffer(uint32_t currentImage);
  std::vector<const char *> getRequiredExtensions();
  int rateDeviceSuitability(VkPhysicalDevice device);
  bool checkDeviceExtensionSupport(VkPhysicalDevice device);
  void endSingleTimeCommands(VkCommandBuffer inputCommandBuffer);
  VkShaderModule createShaderModule(const std::vector<char> &code);
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
  void copyBuffer(VkBuffer inputSrcBuffer, VkBuffer inputDstBuffer, VkDeviceSize inputBufferSize);
  VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

  static void framebufferResizeCallback(GLFWwindow *window, int inputWidth, int inputHeight) {
    auto instanceHandler = reinterpret_cast<vulkan_instance_handler *>(glfwGetWindowUserPointer(window));
    instanceHandler->framebufferResized = true;
  }

  glfw_handler *window;

  static std::vector<char> readFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
      throw std::runtime_error("Failed to open file: " + filename);
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
  }

 public:
  vulkan_instance_handler();
  ~vulkan_instance_handler();

  bool framebufferResized = false;
  void run();
};

#endif
