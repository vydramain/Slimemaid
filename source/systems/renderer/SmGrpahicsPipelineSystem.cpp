/*
------------------------------------
 Slimemaid Source Code (15.06.2022)
 This file is part of Slimemaid Source Code.
----------------------------------
*/

#include "systems/renderer/SmGraphicsPipelineSystem.hpp"

void create_graphics_pipeline(std::string input_vertex_shader_path,
                              std::string input_fragment_shader_path,
                              SmDevices input_devices,
                              SmSwapChain* p_swap_chain,
                              SmDescriptorPool* p_descriptor_pool,
                              SmGraphicsPipeline* p_graphics_pipeline,
                              VkSampleCountFlagBits input_msaa_samples) {
  auto vert_shader_content = readFile(input_vertex_shader_path);
  auto frag_shader_code = readFile(input_fragment_shader_path);

  VkShaderModule vert_shader_module = create_shader_module(input_devices, vert_shader_content);
  VkShaderModule frag_shader_module = create_shader_module(input_devices, frag_shader_code);

  VkPipelineShaderStageCreateInfo vert_shader_stage_info{};
  vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vert_shader_stage_info.module = vert_shader_module;
  vert_shader_stage_info.pName = "main";                 // entrypoint function name of shader module
  vert_shader_stage_info.pSpecializationInfo = nullptr;  // shader variables at render time for compiler optimizations

  VkPipelineShaderStageCreateInfo frag_shader_stage_info{};
  frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  frag_shader_stage_info.module = frag_shader_module;
  frag_shader_stage_info.pName = "main";
  frag_shader_stage_info.pSpecializationInfo = nullptr;

  VkPipelineShaderStageCreateInfo shader_stages[] = {vert_shader_stage_info, frag_shader_stage_info};

  auto binding_description = SmVertex::get_binding_description();
  auto attribute_descriptions = SmVertex::get_attribute_descriptions();

  VkPipelineVertexInputStateCreateInfo vertex_input_info{};
  vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_info.vertexBindingDescriptionCount = 1;
  vertex_input_info.pVertexBindingDescriptions = &binding_description;
  vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
  vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions.data();

  VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info{};
  input_assembly_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly_create_info.primitiveRestartEnable = VK_FALSE;

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.height = (float) p_swap_chain->swap_chain_extent.height;
  viewport.width = (float) p_swap_chain->swap_chain_extent.width;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = p_swap_chain->swap_chain_extent;

  VkPipelineViewportStateCreateInfo viewport_state_create_info{};
  viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state_create_info.viewportCount = 1;
  viewport_state_create_info.pViewports = &viewport;
  viewport_state_create_info.scissorCount = 1;
  viewport_state_create_info.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterization_create_info{};
  rasterization_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterization_create_info.depthClampEnable = VK_FALSE;
  rasterization_create_info.rasterizerDiscardEnable = VK_FALSE;
  rasterization_create_info.polygonMode = VK_POLYGON_MODE_FILL;
  rasterization_create_info.lineWidth = 1.0f;
  rasterization_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterization_create_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterization_create_info.depthBiasEnable = VK_FALSE;
  rasterization_create_info.depthBiasConstantFactor = 0.0f;  // Optional
  rasterization_create_info.depthBiasClamp = 0.0f;           // Optional
  rasterization_create_info.depthBiasSlopeFactor = 0.0f;     // Optional

  VkPipelineMultisampleStateCreateInfo multisample_state_create_info{};
  multisample_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisample_state_create_info.sampleShadingEnable = VK_FALSE;  // enable sample shading in the pipeline
  multisample_state_create_info.rasterizationSamples = input_msaa_samples;
  multisample_state_create_info.minSampleShading = .2f;  // min fraction for sample shading; closer to one is smoother
  multisample_state_create_info.pSampleMask = nullptr;   // Optional
  multisample_state_create_info.alphaToCoverageEnable = VK_FALSE;  // Optional
  multisample_state_create_info.alphaToOneEnable = VK_FALSE;       // Optional

  VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info{};
  depth_stencil_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depth_stencil_state_create_info.depthTestEnable = VK_TRUE;
  depth_stencil_state_create_info.depthWriteEnable = VK_TRUE;
  depth_stencil_state_create_info.depthCompareOp = VK_COMPARE_OP_LESS;
  depth_stencil_state_create_info.depthBoundsTestEnable = VK_FALSE;
  depth_stencil_state_create_info.stencilTestEnable = VK_FALSE;
  depth_stencil_state_create_info.minDepthBounds = 0.0f;  // Optional
  depth_stencil_state_create_info.maxDepthBounds = 1.0f;  // Optional
  depth_stencil_state_create_info.front = {};             // Optional
  depth_stencil_state_create_info.back = {};              // Optional

  VkPipelineColorBlendAttachmentState color_blend_attachment{};
  color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                          VK_COLOR_COMPONENT_G_BIT |
                                          VK_COLOR_COMPONENT_B_BIT |
                                          VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = VK_FALSE;
  color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
  color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

  VkPipelineColorBlendStateCreateInfo color_blend_state_create_info{};
  color_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blend_state_create_info.logicOpEnable = VK_FALSE;
  color_blend_state_create_info.logicOp = VK_LOGIC_OP_COPY;  // Optional
  color_blend_state_create_info.attachmentCount = 1;
  color_blend_state_create_info.pAttachments = &color_blend_attachment;
  color_blend_state_create_info.blendConstants[0] = 0.0f;  // Optional
  color_blend_state_create_info.blendConstants[1] = 0.0f;  // Optional
  color_blend_state_create_info.blendConstants[2] = 0.0f;  // Optional
  color_blend_state_create_info.blendConstants[3] = 0.0f;  // Optional

  // This vector is not used anywhere
  // std::vector<VkDynamicState> dynamicStates = {
  //     VK_DYNAMIC_STATE_VIEWPORT,
  //     VK_DYNAMIC_STATE_LINE_WIDTH
  // };

  VkPipelineLayoutCreateInfo pipeline_layout_create_info{};
  pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_create_info.setLayoutCount = 1;
  pipeline_layout_create_info.pSetLayouts = &p_descriptor_pool->descriptor_set_layout;
  pipeline_layout_create_info.pushConstantRangeCount = 0;     // Optional
  pipeline_layout_create_info.pPushConstantRanges = nullptr;  // Optional

  if (VK_SUCCESS != vkCreatePipelineLayout(input_devices.logical_device,
                                           &pipeline_layout_create_info,
                                           nullptr,
                                           &p_graphics_pipeline->pipeline_layout)) {
    throw std::runtime_error("Failed to create pipeline layout");
  }

  VkGraphicsPipelineCreateInfo pipeline_create_info{};
  pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_create_info.stageCount = 2;
  pipeline_create_info.pStages = shader_stages;
  pipeline_create_info.pVertexInputState = &vertex_input_info;
  pipeline_create_info.pInputAssemblyState = &input_assembly_create_info;
  pipeline_create_info.pViewportState = &viewport_state_create_info;
  pipeline_create_info.pRasterizationState = &rasterization_create_info;
  pipeline_create_info.pMultisampleState = &multisample_state_create_info;
  pipeline_create_info.pDepthStencilState = &depth_stencil_state_create_info;
  pipeline_create_info.pColorBlendState = &color_blend_state_create_info;
  pipeline_create_info.layout = p_graphics_pipeline->pipeline_layout;
  pipeline_create_info.renderPass = p_graphics_pipeline->render_pass;
  pipeline_create_info.subpass = 0;
  pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
  pipeline_create_info.basePipelineIndex = -1;
  pipeline_create_info.pDynamicState = nullptr;

  if (VK_SUCCESS !=vkCreateGraphicsPipelines(input_devices.logical_device,
                                              VK_NULL_HANDLE,
                                              1,
                                              &pipeline_create_info,
                                              nullptr,
                                              &p_graphics_pipeline->pipeline)) {
    throw std::runtime_error("Failed to create graphics pipeline");
  }

  vkDestroyShaderModule(input_devices.logical_device, vert_shader_module, nullptr);
  vkDestroyShaderModule(input_devices.logical_device, frag_shader_module, nullptr);

  std::cout << "Graphics pipeline creation process ends with success..." << std::endl;
}

void create_render_pass(SmDevices input_devices,
                        SmSamplingFlags input_msaa_samples,
                        SmSwapChain* p_swap_chain,
                        SmGraphicsPipeline* p_graphics_pipeline) {
  VkAttachmentDescription color_attachment_description{};
  color_attachment_description.format = p_swap_chain->swap_chain_image_format;
  color_attachment_description.samples = input_msaa_samples.msaa_samples;
  color_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment_description.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription depth_attachment_description{};
  depth_attachment_description.format = find_depth_format(input_devices);
  depth_attachment_description.samples = input_msaa_samples.msaa_samples;
  depth_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depth_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depth_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depth_attachment_description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription color_attachment_resolve_description{};
  color_attachment_resolve_description.format = p_swap_chain->swap_chain_image_format;
  color_attachment_resolve_description.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment_resolve_description.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment_resolve_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment_resolve_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment_resolve_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment_resolve_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment_resolve_description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference color_attachment_reference{};
  color_attachment_reference.attachment = 0;
  color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depth_attachment_reference{};
  depth_attachment_reference.attachment = 1;
  depth_attachment_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference color_attachment_resolve_reference{};
  color_attachment_resolve_reference.attachment = 2;
  color_attachment_resolve_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass_description{};
  subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass_description.colorAttachmentCount = 1;
  subpass_description.pColorAttachments = &color_attachment_reference;
  subpass_description.pDepthStencilAttachment = &depth_attachment_reference;
  subpass_description.pResolveAttachments = &color_attachment_resolve_reference;

  VkSubpassDependency subpass_dependency{};
  subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  subpass_dependency.dstSubpass = 0;
  subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                                    VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  subpass_dependency.srcAccessMask = 0;
  subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                                    VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                                     VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  std::array<VkAttachmentDescription, 3> attachments = {color_attachment_description,
                                                        depth_attachment_description,
                                                        color_attachment_resolve_description};

  VkRenderPassCreateInfo render_pass_create_info{};
  render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
  render_pass_create_info.pAttachments = attachments.data();
  render_pass_create_info.subpassCount = 1;
  render_pass_create_info.pSubpasses = &subpass_description;
  render_pass_create_info.dependencyCount = 1;
  render_pass_create_info.pDependencies = &subpass_dependency;

  if (VK_SUCCESS != vkCreateRenderPass(input_devices.logical_device,
                                       &render_pass_create_info,
                                       nullptr,
                                       &p_graphics_pipeline->render_pass)) {
    throw std::runtime_error("Failed to create render pass");
  }

  std::cout << "Render pass creation project ends with success..." << std::endl;
}
