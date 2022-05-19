/*
------------------------------------
  Slimemaid Source Code (20.05.2022)
  This file is part of Slimemaid Source Code.
  UBO or UniformBufferObject is object that describe every buffered model for any of graphics pipeline needs.
------------------------------------
*/

#ifndef SLIMEMAID_UNIFORMBUFFEROBJECT_HPP
#define SLIMEMAID_UNIFORMBUFFEROBJECT_HPP

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORSE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES // dif
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct UniformBufferObject {
  alignas(16) glm::mat4 model;
  alignas(16) glm::mat4 view;
  alignas(16) glm::mat4 proj;
};

#endif // SLIMEMAID_UNIFORMBUFFEROBJECT_HPP

