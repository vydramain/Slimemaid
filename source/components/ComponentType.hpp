/*
------------------------------------
  Slimemaid Source Code (20.05.2022)
  This file is part of Slimemaid Source Code.
  Component type has a unique ID given to it (for reasons explained later).
------------------------------------
*/

#ifndef SLIMEMAID_COMPONENTTYPE_HPP
#define SLIMEMAID_COMPONENTTYPE_HPP

#include <cstdint> // Necessary for uint32_t

// A simple type alias
using ComponentType = std::uint32_t;

// Used to define the size of arrays later on
const ComponentType MAX_COMPONENTS = 32;

#endif  // SLIMEMAID_COMPONENTTYPE_HPP
