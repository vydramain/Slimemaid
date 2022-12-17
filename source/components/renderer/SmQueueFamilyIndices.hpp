/*
------------------------------------
  Slimemaid Source Code (20.05.2022)
  This file is part of Slimemaid Source Code.
  Every operation in Vulkan, anything from drawing to uploading textures, requires commands to be submitted to a queue.
  There are different types of queues that originate from different queue families
  and each family of queues allows only a subset of commands:
    - Graphics family are used for graphics commands.
    - Present family are used for presentation command.
------------------------------------
*/

#ifndef SLIMEMAID_SMQUEUEFAMILYINDICES_HPP
#define SLIMEMAID_SMQUEUEFAMILYINDICES_HPP

#include <optional>
#include <cstdint> // Necessary for uint32_t

struct SmQueueFamilyIndices {
  std::optional<uint32_t> graphics_family;
  std::optional<uint32_t> present_family;

  [[nodiscard]] bool is_indices_complete() const {
    return graphics_family.has_value() &&
           present_family.has_value();
  }
};

#endif  // SLIMEMAID_SMQUEUEFAMILYINDICES_HPP
