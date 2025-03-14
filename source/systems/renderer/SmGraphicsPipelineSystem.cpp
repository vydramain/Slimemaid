
#include "systems/renderer/SmGraphicsPipelineSystem.hpp"

#include <iostream>

#include "components/renderer/SmDescriptorPool.hpp"
#include "components/renderer/SmVertex.hpp"
#include "systems/reader/SmFlieReaderSystem.hpp"
#include "systems/renderer/SmDepthBuffersSystem.hpp"
#include "systems/renderer/SmShaderSystem.hpp"

void create_graphics_pipeline(const std::string& input_vertex_shader_path,
                              const std::string& input_fragment_shader_path, SmDevices& devices,
                              const SmSwapChain& input_swap_chain,
                              SmDescriptorPool& descriptor_pool,
                              SmGraphicsPipeline& graphics_pipeline,
                              VkSampleCountFlagBits input_msaa_samples) {
  auto vertShaderCode = read_file(input_vertex_shader_path);
  auto fragShaderCode = read_file(input_fragment_shader_path);

  VkShaderModule vertShaderModule = create_shader_module(devices, vertShaderCode);
  VkShaderModule fragShaderModule = create_shader_module(devices, fragShaderCode);

  VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
  vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = vertShaderModule;
  vertShaderStageInfo.pName = "main";  // entrypoint function name of shader module
  vertShaderStageInfo.pSpecializationInfo =
      nullptr;  // shader variables at render time for compiler optimizations

  VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
  fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = fragShaderModule;
  fragShaderStageInfo.pName = "main";
  fragShaderStageInfo.pSpecializationInfo = nullptr;

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

  auto bindingDescription = SmVertex::get_binding_description();
  auto attributeDescriptions = SmVertex::get_attribute_descriptions();

  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = 1;
  vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
  vertexInputInfo.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(attributeDescriptions.size());
  vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

  VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
  inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.height = (float)input_swap_chain.swap_chain_extent.height;
  viewport.width = (float)input_swap_chain.swap_chain_extent.width;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = input_swap_chain.swap_chain_extent;

  VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
  viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportStateCreateInfo.viewportCount = 1;
  viewportStateCreateInfo.pViewports = &viewport;
  viewportStateCreateInfo.scissorCount = 1;
  viewportStateCreateInfo.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo{};
  rasterizationCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizationCreateInfo.depthClampEnable = VK_FALSE;
  rasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE;
  rasterizationCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizationCreateInfo.lineWidth = 1.0f;
  rasterizationCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizationCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizationCreateInfo.depthBiasEnable = VK_FALSE;
  rasterizationCreateInfo.depthBiasConstantFactor = 0.0f;  // Optional
  rasterizationCreateInfo.depthBiasClamp = 0.0f;           // Optional
  rasterizationCreateInfo.depthBiasSlopeFactor = 0.0f;     // Optional

  VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
  multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampleStateCreateInfo.sampleShadingEnable =
      VK_FALSE;  // enable sample shading in the pipeline
  multisampleStateCreateInfo.rasterizationSamples = input_msaa_samples;
  multisampleStateCreateInfo.minSampleShading =
      .2f;  // min fraction for sample shading; closer to one is smoother
  multisampleStateCreateInfo.pSampleMask = nullptr;             // Optional
  multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
  multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;       // Optional

  VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
  depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
  depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
  depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
  depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
  depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
  depthStencilStateCreateInfo.minDepthBounds = 0.0f;  // Optional
  depthStencilStateCreateInfo.maxDepthBounds = 1.0f;  // Optional
  depthStencilStateCreateInfo.front = {};             // Optional
  depthStencilStateCreateInfo.back = {};              // Optional

  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_FALSE;
  colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

  VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
  colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
  colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
  colorBlendStateCreateInfo.attachmentCount = 1;
  colorBlendStateCreateInfo.pAttachments = &colorBlendAttachment;
  colorBlendStateCreateInfo.blendConstants[0] = 0.0f;  // Optional
  colorBlendStateCreateInfo.blendConstants[1] = 0.0f;  // Optional
  colorBlendStateCreateInfo.blendConstants[2] = 0.0f;  // Optional
  colorBlendStateCreateInfo.blendConstants[3] = 0.0f;  // Optional

  // This vector is not used anywhere
  // std::vector<VkDynamicState> dynamicStates = {
  //     VK_DYNAMIC_STATE_VIEWPORT,
  //     VK_DYNAMIC_STATE_LINE_WIDTH
  // };

  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
  pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutCreateInfo.setLayoutCount = 1;
  pipelineLayoutCreateInfo.pSetLayouts = &descriptor_pool.descriptor_set_layout;
  pipelineLayoutCreateInfo.pushConstantRangeCount = 0;     // Optional
  pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;  // Optional

  if (VK_SUCCESS != vkCreatePipelineLayout(devices.logical_device, &pipelineLayoutCreateInfo,
                                           nullptr, &graphics_pipeline.pipeline_layout)) {
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
  pipelineCreateInfo.layout = graphics_pipeline.pipeline_layout;
  pipelineCreateInfo.renderPass = graphics_pipeline.render_pass;
  pipelineCreateInfo.subpass = 0;
  pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
  pipelineCreateInfo.basePipelineIndex = -1;
  pipelineCreateInfo.pDynamicState = nullptr;

  if (VK_SUCCESS != vkCreateGraphicsPipelines(devices.logical_device, VK_NULL_HANDLE, 1,
                                              &pipelineCreateInfo, nullptr,
                                              &graphics_pipeline.pipeline)) {
    throw std::runtime_error("Failed to create graphics pipeline");
  }

  vkDestroyShaderModule(devices.logical_device, vertShaderModule, nullptr);
  vkDestroyShaderModule(devices.logical_device, fragShaderModule, nullptr);

  std::cout << "Graphics pipeline creation process ends with success..." << std::endl;
}

void create_render_pass(SmDevices input_devices, SmSamplingFlags input_msaa_samples,
                        SmGraphicsPipeline* p_graphics_pipeline, SmSwapChain* p_swap_chain) {
  VkAttachmentDescription colorAttachmentDescription{};
  colorAttachmentDescription.format = p_swap_chain->swap_chain_image_format;
  colorAttachmentDescription.samples = input_msaa_samples.msaa_samples;
  colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription depthAttachmentDescription{};
  depthAttachmentDescription.format = find_depth_format(input_devices);
  depthAttachmentDescription.samples = input_msaa_samples.msaa_samples;
  depthAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription colorAttachmentResolveDescription{};
  colorAttachmentResolveDescription.format = p_swap_chain->swap_chain_image_format;
  colorAttachmentResolveDescription.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachmentResolveDescription.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachmentResolveDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachmentResolveDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachmentResolveDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachmentResolveDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachmentResolveDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentReference{};
  colorAttachmentReference.attachment = 0;
  colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthAttachmentReference{};
  depthAttachmentReference.attachment = 1;
  depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference colorAttachmentResolveReference{};
  colorAttachmentResolveReference.attachment = 2;
  colorAttachmentResolveReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

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
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  subpassDependency.srcAccessMask = 0;
  subpassDependency.dstStageMask =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  subpassDependency.dstAccessMask =
      VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  std::array<VkAttachmentDescription, 3> attachments = {
      colorAttachmentDescription, depthAttachmentDescription, colorAttachmentResolveDescription};

  VkRenderPassCreateInfo renderPassCreateInfo{};
  renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  renderPassCreateInfo.pAttachments = attachments.data();
  renderPassCreateInfo.subpassCount = 1;
  renderPassCreateInfo.pSubpasses = &subpassDescription;
  renderPassCreateInfo.dependencyCount = 1;
  renderPassCreateInfo.pDependencies = &subpassDependency;

  if (VK_SUCCESS != vkCreateRenderPass(input_devices.logical_device, &renderPassCreateInfo, nullptr,
                                       &p_graphics_pipeline->render_pass)) {
    throw std::runtime_error("Failed to create render pass");
  }

  std::cout << "Render pass creation project ends with success..." << std::endl;
}
