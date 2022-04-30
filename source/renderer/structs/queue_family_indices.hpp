/*
------------------------------------

Slimemaid Source Code (29.04.2022)

This file is part of Slimemaid Source Code.

This file has heading of allocation functions for Vulkan API.

------------------------------------
*/

#ifndef SM_VULKAN_QUEUE_FAMILY_INDICES_STRUCT_
#define SM_VULKAN_QUEUE_FAMILY_INDICES_STRUCT_

#include <optional>
#include <cstdint>

/*
------------
Stuct keep uint32_t as optional value for graphics and present family.
------------
*/
struct queue_family_indices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool isComplete() {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

#endif
