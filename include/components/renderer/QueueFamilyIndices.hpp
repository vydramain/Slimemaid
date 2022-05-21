/*
------------------------------------
  Slimemaid Source Code (20.05.2022)
  This file is part of Slimemaid Source Code.
  Struct QueueFamilyIndices
------------------------------------
*/

#ifndef SLIMEMAID_QUEUEFAMILYINDICES_HPP
#define SLIMEMAID_QUEUEFAMILYINDICES_HPP

#include <optional>
#include <cstdint> // Necessary for uint32_t

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool isComplete() {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

#endif // SLIMEMAID_QUEUEFAMILYINDICES_HPP

