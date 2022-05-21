/*
------------------------------------
  Slimemaid Source Code (21.05.2022)
  This file is part of Slimemaid Source Code.
  A system is any functionality that iterates upon a list of entities with a certain signature of components.
------------------------------------
*/

#ifndef SLIMEMAID_SYSTEM_HPP
#define SLIMEMAID_SYSTEM_HPP

#include <set>

#include "entities/Entity.hpp"

class System {
 public:
  std::set<Entity> m_entities;
};

#endif  // SLIMEMAID_SYSTEM_HPP
