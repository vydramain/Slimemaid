/*
------------------------------------
  Slimemaid Source Code (04.06.2022)
  This file is part of Slimemaid Source Code.
  Graphics pipeline system can fill appropriate component by input shaders and input settings.
------------------------------------
*/

#ifndef SLIMEMAID_SMGRAPHICSPIPELINESYSTEM_HPP
#define SLIMEMAID_SMGRAPHICSPIPELINESYSTEM_HPP

#include <vulkan/vulkan.h>

#include <cstring>
#include <iostream>

#include "components/renderer/SmGraphicsPipeline.hpp"
#include "components/renderer/SmSwapChain.hpp"
#include "components/renderer/SmVertex.hpp"
#include "systems/reader/SmFlieReaderSystem.hpp"
#include "systems/renderer/SmShaderSystem.hpp"

void createGraphicsPipeline(const std::string& input_vertex_shader_path,
                            const std::string& input_fragment_shader_path,
                            SmDevices& devices,
                            const SmSwapChain& input_swap_chain,
                            SmDescriptorPool& descriptor_pool,
                            SmGraphicsPipeline& graphics_pipeline,
                            VkSampleCountFlagBits input_msaa_samples) {
  auto vertShaderCode = readFile(input_vertex_shader_path);
  auto fragShaderCode = readFile(input_fragment_shader_path);

  VkShaderModule vertShaderModule = createShaderModule(devices, vertShaderCode);
  VkShaderModule fragShaderModule = createShaderModule(devices, fragShaderCode);

  VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
  vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = vertShaderModule;
  vertShaderStageInfo.pName = "main";                 // entrypoint function name of shader module
  vertShaderStageInfo.pSpecializationInfo = nullptr;  // shader variables at render time for compiler optimizations

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
  vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
  vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

  VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
  inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.height = (float) input_swap_chain.swap_chain_extent.height;
  viewport.width = (float) input_swap_chain.swap_chain_extent.width;
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
  multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;  // enable sample shading in the pipeline
  multisampleStateCreateInfo.rasterizationSamples = input_msaa_samples;
  multisampleStateCreateInfo.minSampleShading = .2f;  // min fraction for sample shading; closer to one is smoother
  multisampleStateCreateInfo.pSampleMask = nullptr;   // Optional
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
  colorBlendAttachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
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

  if (VK_SUCCESS != vkCreatePipelineLayout(devices.logical_device,
                                           &pipelineLayoutCreateInfo,
                                           nullptr,
                                           &graphics_pipeline.pipeline_layout)) {
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

  if (VK_SUCCESS !=vkCreateGraphicsPipelines(devices.logical_device,
                                              VK_NULL_HANDLE,
                                              1,
                                              &pipelineCreateInfo,
                                              nullptr,
                                              &graphics_pipeline.pipeline)) {
    throw std::runtime_error("Failed to create graphics pipeline");
  }

  vkDestroyShaderModule(devices.logical_device, vertShaderModule, nullptr);
  vkDestroyShaderModule(devices.logical_device, fragShaderModule, nullptr);

  std::cout << "Graphics pipeline creation process ends with success..." << std::endl;
}

#endif  // SLIMEMAID_SMGRAPHICSPIPELINESYSTEM_HPP
