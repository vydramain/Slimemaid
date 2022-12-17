/*
------------------------------------
  Slimemaid Source Code (19.05.2022)
  This file is part of Slimemaid Source Code.
  Entity is simply an ID. It does not “contain” anything.
  Instead the ID is used as an index into an array of components.
------------------------------------
*/

#ifndef SLIMEMAID_ENTITY_HPP
#define SLIMEMAID_ENTITY_HPP

#include <cstdint> // Necessary for uint32_t

// A simple type alias
using Entity = std::uint32_t;

// Used to define the size of arrays later on
const Entity MAX_ENTITIES = 5000;

#endif  // SLIMEMAID_ENTITY_HPP
