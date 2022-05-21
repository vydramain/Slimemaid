/*
------------------------------------
  Slimemaid Source Code (20.05.2022)
  This file is part of Slimemaid Source Code.
  Interface class for data structure that is essentially a simple array,
  but is always a packed array, meaning it has no holes.
------------------------------------
*/

#ifndef SLIMEMAID_ICOMPONENTARRAY_HPP
#define SLIMEMAID_ICOMPONENTARRAY_HPP

#include "entities/Entity.hpp"

class IComponentArray
{
 public:
  virtual ~IComponentArray() = default;
  virtual void entity_destroyed(Entity entity) = 0;
};

#endif  // SLIMEMAID_ICOMPONENTARRAY_HPP
