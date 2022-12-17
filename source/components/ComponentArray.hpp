/*
------------------------------------
  Slimemaid Source Code (20.05.2022)
  This file is part of Slimemaid Source Code.
  The one instance of virtual inheritance in the entire implementation.
------------------------------------
*/

#ifndef SLIMEMAID_COMPONENTARRAY_HPP
#define SLIMEMAID_COMPONENTARRAY_HPP

#include <array>
#include <cassert>
#include <unordered_map>

#include "components/IComponentArray.hpp"
#include "entities/Entity.hpp"

template<typename T>
class ComponentArray : public IComponentArray {
 public:
  void insert_data(Entity entity, T component);
  void remove_data(Entity entity);
  T& get_data(Entity entity);
  void entity_destroyed(Entity entity) override;

 private:
  // The packed array of components (of generic type T),
  // set to a specified maximum amount, matching the maximum number
  // of entities allowed to exist simultaneously, so that each entity
  // has a unique spot.
  std::array<T, MAX_ENTITIES> m_component_array;

  // Map from an entity ID to an array index.
  std::unordered_map<Entity, std::size_t> m_entity_to_index_map;

  // Map from an array index to an entity ID.
  std::unordered_map<std::size_t, Entity> m_index_to_entity_map;

  // Total size of valid entries in the array.
  std::size_t m_size;
};

#endif  // SLIMEMAID_COMPONENTARRAY_HPP
