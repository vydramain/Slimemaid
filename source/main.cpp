#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORSE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES // dif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <algorithm> // Necessary for std::clamp
#include <array>
#include <chrono>
#include <cstdint> // Necessary for uint32_t
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits> // Necessary for std::numeric_limits
#include <map>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

#include "./renderer/structs/queue_family_indices.hpp"
#include "./renderer/structs/vertex.hpp"
#include "./renderer/allocator.hpp"
#include "./renderer/debug_messenger.hpp"
#include "./renderer/memory_handler.hpp"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

const std::string MODEL_PATH = "raws/viking_room/viking_room.obj";
const std::string TEXTURE_PATH = "raws/viking_room/viking_room.png";

const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

const std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

struct UniformBufferObject {
  alignas(16) glm::mat4 model;
  alignas(16) glm::mat4 view;
  alignas(16) glm::mat4 proj;
};

namespace std {
template <> struct hash<Vertex> {
  size_t operator()(Vertex const &vertex) const {
    return ((hash<glm::vec3>()(vertex.pos) ^
             (hash<glm::vec3>()(vertex.color) << 1)) >>
            1) ^
           (hash<glm::vec2>()(vertex.texCoord) << 1);
  }
};
} // namespace std

class SlimeApplication {
private:
  GLFWwindow *window;

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
  VkDescriptorSetLayout descriptorSetLayout;
  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;

  VkCommandPool commandPool;

  VkImage depthImage;
  VkDeviceMemory depthImageMemory;
  VkImageView depthImageView;

  uint32_t mipLevels;
  VkImage textureImage;
  VkDeviceMemory textureImageMemory;
  VkImageView textureImageView;
  VkSampler textureSampler;

  std::vector<Vertex>
      vertices; // {{rel_x, rel_y, rel_z}, {R, G, B}, {tex_rel_x, tex_rel_x}}
  std::vector<uint32_t> indices;

  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  VkBuffer indexBuffer;
  VkDeviceMemory indexBufferMemory;

  std::vector<VkBuffer> uniformBuffers;
  std::vector<VkDeviceMemory> uniformBuffersMemory;

  VkDescriptorPool descriptorPool;
  std::vector<VkDescriptorSet> descriptorSets;

  std::vector<VkCommandBuffer> commandBuffers;

  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;

  VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

  VkImage colorImage;
  VkDeviceMemory colorImageMemory;
  VkImageView colorImageView;

  bool framebufferResized = false;
  uint32_t currentFrame = 0;

  void initWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    std::cout << "GLFW initialization process ends with success..."
              << std::endl;
  }

  static void framebufferResizeCallback(GLFWwindow *window, int width,
                                        int height) {
    auto app =
        reinterpret_cast<SlimeApplication *>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
  }

  void initVulkan() {
    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createDescriptorSetLayout();
    createGraphicsPipeline();
    createCommandPool();
    createDepthResources();
    createColorResources();
    createFramebuffers();
    createTextureImage();
    createTextureImageView();
    createTextureSampler();
    loadModel();
    createVertexBuffer();
    createIndexBuffer();
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
    createCommandBuffers();
    createSyncObjects();

    std::cout << "Vulkan initialization process ends with success..."
              << std::endl;
  }

  void mainLoop() {
    std::cout << "Start main loop function..." << std::endl;
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();
      drawFrame();
    }

    vkDeviceWaitIdle(device);

    std::cout << "Main loop function stoped..." << std::endl;
  }

  void cleanUpSwapChain() {
    vkDestroyImageView(device, colorImageView, nullptr);
    vkDestroyImage(device, colorImage, nullptr);
    vkFreeMemory(device, colorImageMemory, nullptr);

    vkDestroyImageView(device, depthImageView, nullptr); // dif
    vkDestroyImage(device, depthImage, nullptr);         // dif
    vkFreeMemory(device, depthImageMemory, nullptr);     // dif

    for (auto framebuffer : swapChainFramebuffers) {
      vkDestroyFramebuffer(device, framebuffer, nullptr);
    }

    vkDestroyPipeline(device, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    vkDestroyRenderPass(device, renderPass, nullptr);

    for (auto imageView : swapChainImageViews) {
      vkDestroyImageView(device, imageView, nullptr);
    }

    vkDestroySwapchainKHR(device, swapChain, nullptr);

    std::cout << "Swap chain clean up process ends with success..."
              << std::endl;
  }

  void cleanUp() {
    cleanUpSwapChain();

    vkDestroySampler(device, textureSampler, nullptr);
    vkDestroyImageView(device, textureImageView, nullptr);
    vkDestroyImage(device, textureImage, nullptr);
    vkFreeMemory(device, textureImageMemory, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      vkDestroyBuffer(device, uniformBuffers[i], nullptr);
      vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
    }

    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
    vkDestroyBuffer(device, indexBuffer, nullptr);
    vkFreeMemory(device, indexBufferMemory, nullptr);
    vkDestroyBuffer(device, vertexBuffer, nullptr);
    vkFreeMemory(device, vertexBufferMemory, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
      vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
      vkDestroyFence(device, inFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(device, commandPool, nullptr);
    vkDestroyDevice(device, nullptr);

    if (enableValidationLayers) {
      DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }

    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout << "Clean up process ends with success..." << std::endl;
  }

  void recreateSwapChain() {
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    while (width == 0 || height == 0) {
      glfwGetFramebufferSize(window, &width, &height);
      glfwWaitEvents();
    }

    vkDeviceWaitIdle(device);

    cleanUpSwapChain();

    createSwapChain();
    createImageViews();
    createRenderPass();
    createGraphicsPipeline();
    createDepthResources();
    createColorResources();
    createFramebuffers();
    std::cout << "Swap chain recreation process ends with success..."
              << std::endl;
  }

  void createInstance() {
    if (enableValidationLayers && !checkValidationLayerSupport()) {
      throw std::runtime_error(
          "Validation layers requested, but not avaliable");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Slimemaid";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &appInfo;

    auto glfwExtensions = getRequiredExtensions();
    instanceCreateInfo.enabledExtensionCount =
        static_cast<uint32_t>(glfwExtensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = glfwExtensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if (enableValidationLayers) {
      instanceCreateInfo.enabledLayerCount =
          static_cast<uint32_t>(validationLayers.size());
      instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();

      fillDebugMessengerCreateInfoEXT(debugCreateInfo);
      instanceCreateInfo.pNext =
          (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
    } else {
      instanceCreateInfo.enabledLayerCount = 0;
      instanceCreateInfo.pNext = nullptr;
    }

    if (VK_SUCCESS !=
        vkCreateInstance(&instanceCreateInfo, nullptr, &instance)) {
      throw std::runtime_error("Failed to create Vulkan instance");
    } else {
      std::cout << "Vulkan instance creation process ends with success..."
                << std::endl;
    }
  }

  void fillDebugMessengerCreateInfoEXT(
      VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // Optional
  }

  void setupDebugMessenger() {
    std::cout << "Enable validation layers flag is: " << enableValidationLayers
              << std::endl;
    if (!enableValidationLayers)
      return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    fillDebugMessengerCreateInfoEXT(createInfo);

    if (VK_SUCCESS != CreateDebugUtilsMessengerEXT(instance, &createInfo,
                                                   nullptr, &debugMessenger)) {
      throw std::runtime_error("Failed to set up debug messenger");
    }

    std::cout << "Debug messenger setup process ends with success..."
              << std::endl;
  }

  void createSurface() {
    if (VK_SUCCESS !=
        glfwCreateWindowSurface(instance, window, nullptr, &surface)) {
      throw std::runtime_error("Failed to create window surface");
    }

    std::cout << "GLFW window surface creation process ends with success..."
              << std::endl;
  }

  void pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (0 == deviceCount) {
      throw std::runtime_error("No GPUs with Vulkan support found");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (const auto &device : devices) {
      if (isDeviceSuitable(device)) {
        physicalDevice = device;
        msaaSamples = getMaxUsableSampleCount();
        break;
      }
    }

    if (VK_NULL_HANDLE == physicalDevice) {
      throw std::runtime_error("No suitable GPU found");
    }

    VkPhysicalDeviceProperties chosenDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &chosenDeviceProperties);

    std::cout << "Picking physical device process ends with success..."
              << std::endl
              << "Chosen physical device: " << chosenDeviceProperties.deviceName
              << std::endl;
  }

  VkSampleCountFlagBits getMaxUsableSampleCount() {
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

    VkSampleCountFlags counts =
        physicalDeviceProperties.limits.framebufferColorSampleCounts &
        physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    if (counts & VK_SAMPLE_COUNT_64_BIT) {
      return VK_SAMPLE_COUNT_64_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_32_BIT) {
      return VK_SAMPLE_COUNT_32_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_16_BIT) {
      return VK_SAMPLE_COUNT_16_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_8_BIT) {
      return VK_SAMPLE_COUNT_8_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_4_BIT) {
      return VK_SAMPLE_COUNT_4_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_2_BIT) {
      return VK_SAMPLE_COUNT_2_BIT;
    }

    return VK_SAMPLE_COUNT_1_BIT;
  }

  void createLogicalDevice() {
    QueueFamilyIndices transferIndices =
        findTransferQueueFamilies(physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> transferQueueCreateInfos;
    std::set<uint32_t> transferUniqueQueueFamilies = {
        transferIndices.graphicsFamily.value(),
        transferIndices.presentFamily.value()};

    float transferQueuePriority = 1.0f;
    for (uint32_t queueFamily : transferUniqueQueueFamilies) {
      VkDeviceQueueCreateInfo transferQueueCreateInfo{};
      transferQueueCreateInfo.sType =
          VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      transferQueueCreateInfo.queueFamilyIndex = queueFamily;
      transferQueueCreateInfo.queueCount = 1;
      transferQueueCreateInfo.pQueuePriorities = &transferQueuePriority;
      transferQueueCreateInfos.push_back(transferQueueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.sampleRateShading =
        VK_TRUE; // enable sample shading feature for the device

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount =
        static_cast<uint32_t>(transferQueueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = transferQueueCreateInfos.data();
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount =
        static_cast<uint32_t>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers) {
      deviceCreateInfo.enabledLayerCount =
          static_cast<uint32_t>(validationLayers.size());
      deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
      deviceCreateInfo.enabledLayerCount = 0;
    }

    if (VK_SUCCESS !=
        vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device)) {
      throw std::runtime_error("Failed to create logical device");
    }

    vkGetDeviceQueue(device, transferIndices.graphicsFamily.value(), 0,
                     &graphicsQueue);
    vkGetDeviceQueue(device, transferIndices.presentFamily.value(), 0,
                     &presentQueue);

    std::cout << "Logical device creation process ends with success..."
              << std::endl;
  }

  void createSwapChain() {
    SwapChainSupportDetails swapChainSupport =
        querySwapChainSupport(physicalDevice);

    VkSurfaceFormatKHR surfaceFormat =
        chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode =
        chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D tmpSwapChainExtent =
        chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount) {
      imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = tmpSwapChainExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
                                     indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
      createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
      createInfo.queueFamilyIndexCount = 2;
      createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
      createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
      createInfo.queueFamilyIndexCount = 0;     // Optional
      createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    if (VK_SUCCESS !=
        vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain)) {
      throw std::runtime_error("Failed to create swap chain");
    }

    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount,
                            swapChainImages.data());

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = tmpSwapChainExtent;

    std::cout << "Swap chain creation process ends with success..."
              << std::endl;
  }

  void createImageViews() {
    swapChainImageViews.resize(swapChainImages.size());

    for (uint32_t i = 0; i < swapChainImages.size(); i++) {
      swapChainImageViews[i] =
          createImageView(swapChainImages[i], swapChainImageFormat,
                          VK_IMAGE_ASPECT_COLOR_BIT, 1);
    }

    std::cout << "Image view creation process ends with success..."
              << std::endl;
  }

  void createRenderPass() {
    VkAttachmentDescription colorAttachmentDescription{};
    colorAttachmentDescription.format = swapChainImageFormat;
    colorAttachmentDescription.samples = msaaSamples;
    colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentDescription.stencilStoreOp =
        VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentDescription.finalLayout =
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachmentDescription{};
    depthAttachmentDescription.format = findDepthFormat();
    depthAttachmentDescription.samples = msaaSamples;
    depthAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachmentDescription.stencilStoreOp =
        VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachmentDescription.finalLayout =
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription colorAttachmentResolveDescription{};
    colorAttachmentResolveDescription.format = swapChainImageFormat;
    colorAttachmentResolveDescription.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachmentResolveDescription.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolveDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentResolveDescription.stencilLoadOp =
        VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolveDescription.stencilStoreOp =
        VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentResolveDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentResolveDescription.finalLayout =
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentReference{};
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentReference{};
    depthAttachmentReference.attachment = 1;
    depthAttachmentReference.layout =
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentResolveReference{};
    colorAttachmentResolveReference.attachment = 2;
    colorAttachmentResolveReference.layout =
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescription{};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorAttachmentReference;
    subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;
    subpassDescription.pResolveAttachments = &colorAttachmentResolveReference;

    VkSubpassDependency subpassDependency{};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependency.srcAccessMask = 0;
    subpassDependency.dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependency.dstAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 3> attachments = {
        colorAttachmentDescription, depthAttachmentDescription,
        colorAttachmentResolveDescription};

    VkRenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount =
        static_cast<uint32_t>(attachments.size());
    renderPassCreateInfo.pAttachments = attachments.data();
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpassDescription;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &subpassDependency;

    if (VK_SUCCESS != vkCreateRenderPass(device, &renderPassCreateInfo, nullptr,
                                         &renderPass)) {
      throw std::runtime_error("Failed to create render pass");
    }

    std::cout << "Render pass creation project ends with success..."
              << std::endl;
  }

  void createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType =
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = {
        uboLayoutBinding, samplerLayoutBinding};
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (VK_SUCCESS != vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr,
                                                  &descriptorSetLayout)) {
      throw std::runtime_error("Failed to create descriptor set layout");
    }

    std::cout << "Set layout process description creation ends with success..."
              << std::endl;
  }

  void createGraphicsPipeline() {
    auto vertShaderCode = readFile("./shaders/vert.spv");
    auto fragShaderCode = readFile("./shaders/frag.spv");

    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName =
        "main"; // entrypoint function name of shader module
    vertShaderStageInfo.pSpecializationInfo =
        nullptr; // shader variables at render time for compiler optimizations

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";
    fragShaderStageInfo.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                      fragShaderStageInfo};

    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount =
        static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
    inputAssemblyCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.height = (float)swapChainExtent.height;
    viewport.width = (float)swapChainExtent.width;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChainExtent;

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
    viewportStateCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pViewports = &viewport;
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo{};
    rasterizationCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationCreateInfo.depthClampEnable = VK_FALSE;
    rasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationCreateInfo.lineWidth = 1.0f;
    rasterizationCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationCreateInfo.depthBiasConstantFactor = 0.0f; // Optional
    rasterizationCreateInfo.depthBiasClamp = 0.0f;          // Optional
    rasterizationCreateInfo.depthBiasSlopeFactor = 0.0f;    // Optional

    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
    multisampleStateCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleStateCreateInfo.sampleShadingEnable =
        VK_FALSE; // enable sample shading in the pipeline
    multisampleStateCreateInfo.rasterizationSamples = msaaSamples;
    multisampleStateCreateInfo.minSampleShading =
        .2f; // min fraction for sample shading; closer to one is smoother
    multisampleStateCreateInfo.pSampleMask = nullptr;            // Optional
    multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;      // Optional

    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
    depthStencilStateCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
    depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
    depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.minDepthBounds = 0.0f; // Optional
    depthStencilStateCreateInfo.maxDepthBounds = 1.0f; // Optional
    depthStencilStateCreateInfo.front = {};            // Optional
    depthStencilStateCreateInfo.back = {};             // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor =
        VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
    colorBlendStateCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlendStateCreateInfo.attachmentCount = 1;
    colorBlendStateCreateInfo.pAttachments = &colorBlendAttachment;
    colorBlendStateCreateInfo.blendConstants[0] = 0.0f; // Optional
    colorBlendStateCreateInfo.blendConstants[1] = 0.0f; // Optional
    colorBlendStateCreateInfo.blendConstants[2] = 0.0f; // Optional
    colorBlendStateCreateInfo.blendConstants[3] = 0.0f; // Optional

    // This vector is not used anywhere
    // std::vector<VkDynamicState> dynamicStates = {
    //     VK_DYNAMIC_STATE_VIEWPORT,
    //     VK_DYNAMIC_STATE_LINE_WIDTH
    // };

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;    // Optional
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr; // Optional

    if (VK_SUCCESS != vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo,
                                             nullptr, &pipelineLayout)) {
      throw std::runtime_error("Failed to create pipeline layout");
    }

    VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.stageCount = 2;
    pipelineCreateInfo.pStages = shaderStages;
    pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizationCreateInfo;
    pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
    pipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
    pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
    pipelineCreateInfo.layout = pipelineLayout;
    pipelineCreateInfo.renderPass = renderPass;
    pipelineCreateInfo.subpass = 0;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = -1;
    pipelineCreateInfo.pDynamicState = nullptr;

    if (VK_SUCCESS != vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1,
                                                &pipelineCreateInfo, nullptr,
                                                &graphicsPipeline)) {
      throw std::runtime_error("Failed to create graphics pipeline");
    }

    vkDestroyShaderModule(device, vertShaderModule, nullptr);
    vkDestroyShaderModule(device, fragShaderModule, nullptr);

    std::cout << "Graphics pipeline creation process ends with success..."
              << std::endl;
  }

  void createFramebuffers() {
    swapChainFramebuffers.resize(swapChainImageViews.size());
    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
      std::array<VkImageView, 3> attachments = {
          colorImageView,
          depthImageView,
          swapChainImageViews[i],
      };

      VkFramebufferCreateInfo framebufferCreateInfo{};
      framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      framebufferCreateInfo.renderPass = renderPass;
      framebufferCreateInfo.attachmentCount =
          static_cast<uint32_t>(attachments.size());
      framebufferCreateInfo.pAttachments = attachments.data();
      framebufferCreateInfo.width = swapChainExtent.width;
      framebufferCreateInfo.height = swapChainExtent.height;
      framebufferCreateInfo.layers = 1;

      if (VK_SUCCESS != vkCreateFramebuffer(device, &framebufferCreateInfo,
                                            nullptr,
                                            &swapChainFramebuffers[i])) {
        throw std::runtime_error("Failed to create framebuffer");
      }
    }

    std::cout << "Framebuffers creation and implementation processs ends with "
                 "success..."
              << std::endl;
  }

  void createCommandPool() {
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

    VkCommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags =
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolCreateInfo.queueFamilyIndex =
        queueFamilyIndices.graphicsFamily.value();

    if (VK_SUCCESS != vkCreateCommandPool(device, &commandPoolCreateInfo,
                                          nullptr, &commandPool)) {
      throw std::runtime_error("Failed to create command pool");
    }

    std::cout << "Command pool creation process ends with success..."
              << std::endl;
  }

  void createDepthResources() {
    VkFormat depthFormat = findDepthFormat();

    createImage(swapChainExtent.width, swapChainExtent.height, 1, msaaSamples,
                depthFormat, VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage,
                depthImageMemory, physicalDevice, device);
    depthImageView =
        createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
    transitionImageLayout(depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);

    std::cout << "Depth resources creation process ends with success..."
              << std::endl;
  }

  VkFormat findDepthFormat() {
    return findSupportedDepthFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
         VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
  }

  VkFormat findSupportedDepthFormat(const std::vector<VkFormat> &candidates,
                                    VkImageTiling inputTiling,
                                    VkFormatFeatureFlags inputFlags) {
    for (VkFormat format : candidates) {
      VkFormatProperties deviceProperties;
      vkGetPhysicalDeviceFormatProperties(physicalDevice, format,
                                          &deviceProperties);

      if (VK_IMAGE_TILING_LINEAR == inputTiling &&
          (deviceProperties.linearTilingFeatures & inputFlags) == inputFlags) {
        return format;
      } else if (VK_IMAGE_TILING_OPTIMAL == inputTiling &&
                 (deviceProperties.optimalTilingFeatures & inputFlags) ==
                     inputFlags) {
        return format;
      }
    }

    throw std::runtime_error("Failed to find supported depth format");
  }

  void createColorResources() {
    VkFormat colorFormat = swapChainImageFormat;

    createImage(swapChainExtent.width, swapChainExtent.height, 1, msaaSamples,
                colorFormat, VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
                    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, colorImage,
                colorImageMemory, physicalDevice, device);
    colorImageView =
        createImageView(colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
  }

  void createTextureImage() {
    int textureHeight, textureWidth, textureChannels;

    stbi_uc *pixels =
        stbi_load(TEXTURE_PATH.c_str(), &textureWidth, &textureHeight,
                  &textureChannels, STBI_rgb_alpha);

    VkDeviceSize imageSize = textureWidth * textureHeight * 4;
    mipLevels = static_cast<uint32_t>(std::floor(
                    std::log2(std::max(textureWidth, textureHeight)))) +
                1;

    if (!pixels) {
      throw std::runtime_error("Failed to load raw texture from '" +
                               TEXTURE_PATH + "'");
    } else {
      std::cout << "Texture image '" + TEXTURE_PATH +
                       "' was imported with success..."
                << std::endl;
    }

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);

    void *data;
    vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(device, stagingBufferMemory);

    stbi_image_free(pixels);

    createImage(textureWidth, textureHeight, mipLevels, VK_SAMPLE_COUNT_1_BIT,
                VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                    VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                    VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage,
                textureImageMemory, physicalDevice, device);

    transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB,
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
    copyBufferToImage(stagingBuffer, textureImage,
                      static_cast<size_t>(textureWidth),
                      static_cast<size_t>(textureHeight));
    // transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating
    // mipmaps

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);

    generateMipmaps(textureImage, VK_FORMAT_R8G8B8A8_SRGB, textureWidth,
                    textureHeight, mipLevels);

    std::cout << "Texture image transition process ends with success..."
              << std::endl;
  }

  void createTextureImageView() {
    textureImageView = createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB,
                                       VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);

    std::cout << "Filling texture image view process ends with success..."
              << std::endl;
  }

  void transitionImageLayout(VkImage inputImage, VkFormat inputFormat,
                             VkImageLayout inputOldImageLayout,
                             VkImageLayout inputNewImageLayout,
                             uint32_t inputMipLevels) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier imageMemoryBarrier{};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.oldLayout = inputOldImageLayout;
    imageMemoryBarrier.newLayout = inputNewImageLayout;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.image = inputImage;
    imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
    imageMemoryBarrier.subresourceRange.levelCount = inputMipLevels;
    imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
    imageMemoryBarrier.subresourceRange.layerCount = 1;
    if (VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ==
        inputNewImageLayout) {
      imageMemoryBarrier.subresourceRange.aspectMask =
          VK_IMAGE_ASPECT_DEPTH_BIT;

      if (hasStencilComponent(inputFormat)) {
        imageMemoryBarrier.subresourceRange.aspectMask |=
            VK_IMAGE_ASPECT_STENCIL_BIT;
      }
    } else {
      imageMemoryBarrier.subresourceRange.aspectMask =
          VK_IMAGE_ASPECT_COLOR_BIT;
    }

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (VK_IMAGE_LAYOUT_UNDEFINED == inputOldImageLayout &&
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL == inputNewImageLayout) {
      imageMemoryBarrier.srcAccessMask = 0;
      imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

      sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
      destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL == inputOldImageLayout &&
               VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ==
                   inputNewImageLayout) {
      imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

      sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
      destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if (VK_IMAGE_LAYOUT_UNDEFINED == inputOldImageLayout &&
               VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ==
                   inputNewImageLayout) {
      imageMemoryBarrier.srcAccessMask = 0;
      imageMemoryBarrier.dstAccessMask =
          VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
          VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

      sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
      destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    } else {
      throw std::invalid_argument("Unsupported layout transition");
    }

    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0,
                         nullptr, 0, nullptr, 1, &imageMemoryBarrier);

    endSingleTimeCommands(commandBuffer);
  }

  void generateMipmaps(VkImage inputImage, VkFormat inputImageFormat,
                       uint32_t inputTextureWidth, uint32_t inputTextureHeight,
                       uint32_t inputMipLevels) {

    // Check if  image format supports linear blitting
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(physicalDevice, inputImageFormat,
                                        &formatProperties);

    if (!(formatProperties.optimalTilingFeatures &
          VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
      throw std::runtime_error(
          "Texture image format does not support linear blitting");
    }

    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = inputImage;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.layerCount = 1;

    int32_t mipWidth = inputTextureWidth;
    int32_t mipHeight = inputTextureHeight;

    for (uint32_t i = 1; i < inputMipLevels; i++) {
      barrier.subresourceRange.baseMipLevel = i - 1;
      barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
      barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

      vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                           VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
                           nullptr, 1, &barrier);

      VkImageBlit blit{};
      blit.srcOffsets[0] = {0, 0, 0};
      blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
      blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      blit.srcSubresource.mipLevel = i - 1;
      blit.srcSubresource.baseArrayLayer = 0;
      blit.srcSubresource.layerCount = 1;

      int32_t dstMipWidth = 1;
      if (mipWidth > 1) {
        dstMipWidth = mipWidth / 2;
      }

      int32_t dstMipHeight = 1;
      if (mipHeight > 1) {
        dstMipHeight = mipHeight / 2;
      }

      blit.dstOffsets[0] = {0, 0, 0};
      blit.dstOffsets[1] = {dstMipWidth, dstMipHeight, 1};
      blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      blit.dstSubresource.mipLevel = i;
      blit.dstSubresource.baseArrayLayer = 0;
      blit.dstSubresource.layerCount = 1;

      vkCmdBlitImage(commandBuffer, inputImage,
                     VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, inputImage,
                     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit,
                     VK_FILTER_LINEAR);

      barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
      barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
      barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

      vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                           VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr,
                           0, nullptr, 1, &barrier);

      if (mipHeight > 1) {
        mipWidth /= 2;
      }
      if (mipWidth > 1) {
        mipHeight /= 2;
      }
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr,
                         0, nullptr, 1, &barrier);

    endSingleTimeCommands(commandBuffer);
  }

  bool hasStencilComponent(VkFormat format) {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
           format == VK_FORMAT_D24_UNORM_S8_UINT;
  }

  VkImageView createImageView(VkImage inputImage, VkFormat inputFormat,
                              VkImageAspectFlags inputAspectMask,
                              uint32_t inputMipLevels) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = inputImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = inputFormat;
    viewInfo.subresourceRange.aspectMask = inputAspectMask;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = inputMipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) !=
        VK_SUCCESS) {
      throw std::runtime_error("Failed to create texture image view");
    }

    return imageView;
  }

  void createTextureSampler() {
    VkPhysicalDeviceProperties deviceProperties{};
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    VkSamplerCreateInfo samplerCreateInfo{};
    samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
    samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
    samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.anisotropyEnable = VK_TRUE;
    samplerCreateInfo.maxAnisotropy =
        deviceProperties.limits.maxSamplerAnisotropy;
    samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
    samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
    samplerCreateInfo.compareEnable = VK_FALSE;
    samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerCreateInfo.mipLodBias = 0.0f;
    samplerCreateInfo.minLod = 0.0f;
    samplerCreateInfo.maxLod = static_cast<float>(mipLevels);

    if (VK_SUCCESS !=
        vkCreateSampler(device, &samplerCreateInfo, nullptr, &textureSampler)) {
      throw std::runtime_error("Failed to create texture sampler");
    }

    std::cout << "Texture sampler creation process ends with success..."
              << std::endl;
  }

  void loadModel() {
    tinyobj::attrib_t attribute;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attribute, &shapes, &materials, &warn, &err,
                          MODEL_PATH.c_str())) {
      throw std::runtime_error("Failed to load model from '" + MODEL_PATH +
                               "' because: " + warn + err);
    } else {
      std::cout << "Model from '" + MODEL_PATH + "' was loaded with success..."
                << std::endl;
    }

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    for (const auto &shape : shapes) {
      for (const auto &index : shape.mesh.indices) {
        Vertex vertex{};

        vertex.pos = {attribute.vertices[3 * index.vertex_index + 0],
                      attribute.vertices[3 * index.vertex_index + 1],
                      attribute.vertices[3 * index.vertex_index + 2]};

        vertex.texCoord = {
            attribute.texcoords[2 * index.texcoord_index + 0],
            1.0f - attribute.texcoords[2 * index.texcoord_index + 1]};

        vertex.color = {1.0f, 1.0f, 1.0f};

        if (uniqueVertices.count(vertex) == 0) {
          uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
          vertices.push_back(vertex);
        }

        indices.push_back(uniqueVertices[vertex]);
      }
    }
  }

  void copyBufferToImage(VkBuffer inputBuffer, VkImage inputImage,
                         uint32_t inputWidth, uint32_t inputHeight) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferImageCopy imageRegion{};
    imageRegion.bufferOffset = 0;
    imageRegion.bufferRowLength = 0;
    imageRegion.bufferImageHeight = 0;
    imageRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageRegion.imageSubresource.mipLevel = 0;
    imageRegion.imageSubresource.baseArrayLayer = 0;
    imageRegion.imageSubresource.layerCount = 1;
    imageRegion.imageOffset = {0, 0, 0};
    imageRegion.imageExtent = {inputWidth, inputHeight, 1};

    vkCmdCopyBufferToImage(commandBuffer, inputBuffer, inputImage,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                           &imageRegion);

    endSingleTimeCommands(commandBuffer);
  }

  void createVertexBuffer() {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);

    void *data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

    copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);

    std::cout << "Vertex buffer creation process ends with success..."
              << std::endl;
  }

  void createIndexBuffer() {
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);

    void *data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

    copyBuffer(stagingBuffer, indexBuffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);

    std::cout << "Index buffer creation process ends with success..."
              << std::endl;
  }

  void createUniformBuffers() {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                   uniformBuffers[i], uniformBuffersMemory[i]);
    }

    std::cout << "Uniform buffer creation process ends with success..."
              << std::endl;
  }

  void createBuffer(VkDeviceSize inputSize, VkBufferUsageFlags inputUsage,
                    VkMemoryPropertyFlags inputMemoryProperties,
                    VkBuffer &buffer, VkDeviceMemory &bufferMemory) {

    VkBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = inputSize;
    bufferCreateInfo.usage = inputUsage;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (VK_SUCCESS !=
        vkCreateBuffer(device, &bufferCreateInfo, nullptr, &buffer)) {
      throw std::runtime_error("Failed to create buffer");
    }

    VkMemoryRequirements bufferMemRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &bufferMemRequirements);

    VkMemoryAllocateInfo memAllocateInfo{};
    memAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocateInfo.allocationSize = bufferMemRequirements.size;
    memAllocateInfo.memoryTypeIndex =
        findMemoryType(physicalDevice, bufferMemRequirements.memoryTypeBits,
                       inputMemoryProperties);

    if (VK_SUCCESS !=
        vkAllocateMemory(device, &memAllocateInfo, nullptr, &bufferMemory)) {
      throw std::runtime_error("Failed to allocate buffer memory");
    }

    vkBindBufferMemory(device, buffer, bufferMemory, 0);
  }

  void copyBuffer(VkBuffer inputSrcBuffer, VkBuffer inputDstBuffer,
                  VkDeviceSize inputBufferSize) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = inputBufferSize;
    vkCmdCopyBuffer(commandBuffer, inputSrcBuffer, inputDstBuffer, 1,
                    &copyRegion);

    endSingleTimeCommands(commandBuffer);
  }

  void createDescriptorPool() {
    std::array<VkDescriptorPoolSize, 2> descriptorPoolSizes{};
    descriptorPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorPoolSizes[0].descriptorCount =
        static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    descriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorPoolSizes[1].descriptorCount =
        static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
    descriptorPoolCreateInfo.sType =
        VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.poolSizeCount =
        static_cast<uint32_t>(descriptorPoolSizes.size());
    descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();
    descriptorPoolCreateInfo.maxSets =
        static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    if (VK_SUCCESS != vkCreateDescriptorPool(device, &descriptorPoolCreateInfo,
                                             nullptr, &descriptorPool)) {
      throw std::runtime_error(
          "Failed to create descriptor pool for uniform buffers");
    }

    std::cout << "Description pool creation process ends with success..."
              << std::endl;
  }

  void createDescriptorSets() {
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT,
                                               descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) !=
        VK_SUCCESS) {
      throw std::runtime_error(
          "Failed to allocate descriptor sets for uniform buffers");
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      VkDescriptorBufferInfo bufferInfo{};
      bufferInfo.buffer = uniformBuffers[i];
      bufferInfo.offset = 0;
      bufferInfo.range = sizeof(UniformBufferObject);

      VkDescriptorImageInfo imageInfo{};
      imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      imageInfo.imageView = textureImageView;
      imageInfo.sampler = textureSampler;

      std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
      descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrites[0].dstSet = descriptorSets[i];
      descriptorWrites[0].dstBinding = 0;
      descriptorWrites[0].dstArrayElement = 0;
      descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      descriptorWrites[0].descriptorCount = 1;
      descriptorWrites[0].pBufferInfo = &bufferInfo;

      descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrites[1].dstSet = descriptorSets[i];
      descriptorWrites[1].dstBinding = 1;
      descriptorWrites[1].dstArrayElement = 0;
      descriptorWrites[1].descriptorType =
          VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      descriptorWrites[1].descriptorCount = 1;
      descriptorWrites[1].pImageInfo = &imageInfo;

      vkUpdateDescriptorSets(device,
                             static_cast<uint32_t>(descriptorWrites.size()),
                             descriptorWrites.data(), 0, nullptr);
    }

    std::cout << "Descriptor sets creation process ends with success..."
              << std::endl;
  }

  void createCommandBuffers() {
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType =
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount =
        (uint32_t)commandBuffers.size();

    if (VK_SUCCESS != vkAllocateCommandBuffers(device,
                                               &commandBufferAllocateInfo,
                                               commandBuffers.data())) {
      throw std::runtime_error("Failed to allocate command buffers");
    }

    std::cout << "Command buffers creation process ends with success..."
              << std::endl;
  }

  void createSyncObjects() {
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreCreateInfo{};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      if (VK_SUCCESS != vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr,
                                          &imageAvailableSemaphores[i]) ||
          VK_SUCCESS != vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr,
                                          &renderFinishedSemaphores[i]) ||
          VK_SUCCESS != vkCreateFence(device, &fenceCreateInfo, nullptr,
                                      &inFlightFences[i])) {
        throw std::runtime_error("Failed to create semaphores or fence");
      }
    }

    std::cout << "Sync objects creation process ends with success..."
              << std::endl;
  }

  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData) {
    std::cerr << pCallbackData->pMessage << '\n';

    return VK_FALSE;
  }

  static std::vector<char> readFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
      throw std::runtime_error("Failed to open file: " + filename);
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
  }

  std::vector<const char *> getRequiredExtensions() {
    uint32_t glfwExtensionsCount = 0;
    const char **glfwExtensions =
        glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

    std::vector<const char *> extensions(glfwExtensions,
                                         glfwExtensions + glfwExtensionsCount);

    if (enableValidationLayers) {
      extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
  }

  bool checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char *layerName : validationLayers) {
      bool layerFound = false;
      for (const auto &layerProperties : availableLayers) {
        if (0 == strcmp(layerName, layerProperties.layerName)) {
          layerFound = true;
          break;
        }
      }
      if (!layerFound) {
        return false;
      }
    }

    return true;
  }

  VkCommandBuffer beginSingleTimeCommands() {
    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType =
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &commandBufferAllocateInfo,
                             &commandBuffer);

    VkCommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

    return commandBuffer;
  }

  void endSingleTimeCommands(VkCommandBuffer inputCommandBuffer) {
    vkEndCommandBuffer(inputCommandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &inputCommandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(device, commandPool, 1, &inputCommandBuffer);
  }

  VkShaderModule createShaderModule(const std::vector<char> &code) {
    VkShaderModuleCreateInfo shaderCreateInfo{};
    shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderCreateInfo.codeSize = code.size();
    shaderCreateInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    VkShaderModule shaderModule;
    if (VK_SUCCESS != vkCreateShaderModule(device, &shaderCreateInfo, nullptr,
                                           &shaderModule)) {
      throw std::runtime_error("Failed to create shader module");
    }

    return shaderModule;
  }

  int rateDeviceSuitability(VkPhysicalDevice device) {
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;

    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

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

  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                             nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                             queueFamilies.data());

    int i = 0;
    for (const auto &queueFamily : queueFamilies) {
      if (indices.isComplete()) {
        break;
      }
      if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        indices.graphicsFamily = i;
      }
      VkBool32 presentSupport = false;
      vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
      if (presentSupport) {
        indices.presentFamily = i;
      }

      i++;
    }

    return indices;
  }

  QueueFamilyIndices findTransferQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                             nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                             queueFamilies.data());

    int i = 0;
    for (const auto &queueFamily : queueFamilies) {
      if (indices.isComplete()) {
        break;
      }
      if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) {
        indices.graphicsFamily = i;
      }
      VkBool32 presentSupport = false;
      vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
      if (presentSupport) {
        indices.presentFamily = i;
      }

      i++;
    }

    return indices;
  }

  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
                                              &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
                                         nullptr);
    if (0 != formatCount) {
      details.formats.resize(formatCount);
      vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
                                           details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface,
                                              &presentModeCount, nullptr);

    if (0 != presentModeCount) {
      details.presentModes.resize(presentModeCount);
      vkGetPhysicalDeviceSurfacePresentModesKHR(
          device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
  }

  VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats) {
    for (const auto &availableFormat : availableFormats) {
      if (VK_FORMAT_B8G8R8A8_SRGB == availableFormat.format &&
          VK_COLORSPACE_SRGB_NONLINEAR_KHR == availableFormat.colorSpace) {
        return availableFormat;
      }
    }
    return availableFormats[0];
  }

  VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes) {
    for (const auto &availablePresentMode : availablePresentModes) {
      if (VK_PRESENT_MODE_MAILBOX_KHR == availablePresentMode) {
        return availablePresentMode;
      }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
  }

  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
    if (std::numeric_limits<uint32_t>::max() !=
        capabilities.currentExtent.width) {
      return capabilities.currentExtent;
    } else {
      int height;
      int width;
      glfwGetFramebufferSize(window, &height, &width);

      VkExtent2D actualExtent = {static_cast<uint32_t>(height),
                                 static_cast<uint32_t>(width)};

      actualExtent.width =
          std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                     capabilities.maxImageExtent.width);
      actualExtent.height =
          std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                     capabilities.maxImageExtent.height);

      return actualExtent;
    }
  }

  bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                         nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                         availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(),
                                             deviceExtensions.end());

    for (const auto &extension : availableExtensions) {
      requiredExtensions.erase(extension.extensionName);
    }
    return requiredExtensions.empty();
  }

  bool isDeviceSuitable(VkPhysicalDevice device) {
    QueueFamilyIndices indices = findQueueFamilies(device);
    bool extensionsSupported = checkDeviceExtensionSupport(device);
    bool swapChainAdequate = false;

    if (extensionsSupported) {
      SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
      swapChainAdequate = !swapChainSupport.formats.empty() &&
                          !swapChainSupport.presentModes.empty();
    }

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    return indices.isComplete() && extensionsSupported && swapChainAdequate &&
           supportedFeatures.samplerAnisotropy;
  }

  void drawFrame() {
    // Create the fence in the signaled state, so that the first call to
    // vkWaitForFences() returns immediately since the fence is already
    // signaled. This builted into the API. This behavior reached by flag =
    // VK_FENCE_CREATE_SIGNALED_BIT.
    vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE,
                    UINT64_MAX);

    uint32_t imageIndex;
    VkResult acquireImageResult = vkAcquireNextImageKHR(
        device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame],
        VK_NULL_HANDLE, &imageIndex);
    if (VK_ERROR_OUT_OF_DATE_KHR == acquireImageResult) {
      recreateSwapChain();
      return;
    } else if (VK_SUCCESS != acquireImageResult &&
               VK_SUBOPTIMAL_KHR != acquireImageResult) {
      throw std::runtime_error("Failed to acquired swap chain image");
    }

    vkResetFences(device, 1,
                  &inFlightFences[currentFrame]); // Only reset the fence if we
                                                  // are submitting work

    vkResetCommandBuffer(commandBuffers[currentFrame], 0);
    recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    updateUniformBuffer(currentFrame);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[currentFrame];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (VK_SUCCESS != vkQueueSubmit(graphicsQueue, 1, &submitInfo,
                                    inFlightFences[currentFrame])) {
      throw std::runtime_error("Failed to submit draw command buffer");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    VkResult queuePresentResult = vkQueuePresentKHR(presentQueue, &presentInfo);
    if (VK_ERROR_OUT_OF_DATE_KHR == queuePresentResult ||
        VK_SUBOPTIMAL_KHR == queuePresentResult || framebufferResized) {
      framebufferResized = false;
      recreateSwapChain();
    } else if (VK_SUCCESS != queuePresentResult) {
      throw std::runtime_error("Failed to preset swap chain image");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
  }

  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo bufferBeginInfo{};
    bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    bufferBeginInfo.flags = 0;                  // Optional
    bufferBeginInfo.pInheritanceInfo = nullptr; // optional

    if (VK_SUCCESS != vkBeginCommandBuffer(commandBuffer, &bufferBeginInfo)) {
      throw std::runtime_error("Failed to begin recording command buffer");
    }

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.framebuffer = swapChainFramebuffers[imageIndex];
    renderPassBeginInfo.renderArea.offset = {0, 0};
    renderPassBeginInfo.renderArea.extent = swapChainExtent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    renderPassBeginInfo.clearValueCount =
        static_cast<uint32_t>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo,
                         VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      graphicsPipeline);

    VkBuffer vertexBuffers[] = {vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipelineLayout, 0, 1, descriptorSets.data(), 0,
                            nullptr);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0,
                     0, 0);

    vkCmdEndRenderPass(commandBuffer);

    // Documentation's description for vkCmdDraw function. Parameters:
    // vertexCount - Even though we don't have a vertex buffer, we technically
    // still have 3 vertices to draw. instanceCount - Used for instanced
    // rendering, use 1 if you're not doing that. firstVertex - Used as an
    // offset into the vertex buffer, defines the lowest value of
    // gl_VertexIndex. firstInstance - Used as an offset for instanced
    // rendering, defines the lowest value of gl_InstanceIndex.

    if (VK_SUCCESS != vkEndCommandBuffer(commandBuffer)) {
      throw std::runtime_error("Failed to record command buffer");
    }
  }

  void updateUniformBuffer(uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(
                     currentTime - startTime)
                     .count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(20.0f),
                            glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view =
        glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(
        glm::radians(45.0f),
        swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    // Bug reason is:
    void *data;
    vkMapMemory(device, uniformBuffersMemory.at(currentImage), 0,
                (VkDeviceSize)sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(device, uniformBuffersMemory.at(currentImage));
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
  SlimeApplication app;

  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
