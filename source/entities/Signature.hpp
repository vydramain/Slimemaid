/*
------------------------------------
  Slimemaid Source Code (20.05.2022)
  This file is part of Slimemaid Source Code.
  Since an entity is simply an ID, we need a way to track which components an entity “has”,
  and we also need a way to track which components a system cares about.
  Each component type has a unique ID (starting from 0), which is used to represent a bit in the signature.
------------------------------------
*/

#ifndef SLIMEMAID_SIGNTURE_HPP
#define SLIMEMAID_SIGNTURE_HPP

#include <bitset>

#include "components/ComponentType.hpp"

// A simple type alias
using Signature = std::bitset<MAX_COMPONENTS>;

#endif  // SLIMEMAID_SIGNTURE_HPP
