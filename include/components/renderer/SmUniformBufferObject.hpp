/*
------------------------------------
  Slimemaid Source Code (20.05.2022)
  This file is part of Slimemaid Source Code.
  A model-view-projection matrix for 3D graphics is required to describe the component. Uniform buffer object is object
  that describe every buffered model for graphics pipeline needs.
  UBO (Uniform buffer object) is raw loaded model for transformations that descriptors can make.
------------------------------------
*/

#ifndef SLIMEMAID_SMUNIFORMBUFFEROBJECT_HPP
#define SLIMEMAID_SMUNIFORMBUFFEROBJECT_HPP

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORSE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES // dif
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct SmUniformBufferObject {
  alignas(16) glm::mat4 model;
  alignas(16) glm::mat4 view;
  alignas(16) glm::mat4 proj;
};

#endif  // SLIMEMAID_SMUNIFORMBUFFEROBJECT_HPP
