/*
------------------------------------
  Slimemaid Source Code (04.06.2022)
  This file is part of Slimemaid Source Code.
  Graphic pipeline keeps objects for describing and setting render conditions:
    - A render pass object represents a collection of attachments, subpasses, and dependencies between the subpasses,
      and describes how the attachments are used over the course of the subpasses.
    - Access to descriptor sets from a pipeline is accomplished through a pipeline layout. Descriptors are describing
      through pipeline layout objects the complete set of resources that can be accessed by a pipeline.
      The pipeline layout represents a sequence of descriptor sets with each having a specific layout.
      This sequence of layouts is used to determine the interface between shader stages and shader resources.
      Each pipeline is created using a pipeline layout.
    - Compute pipelines consist of a single static compute shader stage and the pipeline layout.
    - A descriptor set layout object is defined by an array of zero or more descriptor bindings.
      Each individual descriptor binding is specified by a descriptor type, a count (array size)
      of the number of descriptors in the binding, a set of shader stages that can access the binding,
      and (if using immutable samplers) an array of sampler descriptors.
------------------------------------
*/

#ifndef SLIMEMAID_SMGRAPHICSPIPELINE_HPP
#define SLIMEMAID_SMGRAPHICSPIPELINE_HPP

#include <vulkan/vulkan.h>

struct SmGraphicsPipeline {
  VkRenderPass render_pass;
  VkPipeline pipeline;
  VkPipelineLayout pipeline_layout;
};

#endif  // SLIMEMAID_SMGRAPHICSPIPELINE_HPP
