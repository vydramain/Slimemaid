/*
------------------------------------
  Slimemaid Source Code (21.05.2022)
  This file is part of Slimemaid Source Code.
  Component Manager is in charge of talking to all of different ComponentArrays
  when a component needs to be added or removed.
------------------------------------
*/

#ifndef SLIMEMAID_COMPONENTMANAGER_HPP
#define SLIMEMAID_COMPONENTMANAGER_HPP

#include <memory>
#include <cassert>

#include "entities/Entity.hpp"
#include "components/ComponentType.hpp"
#include "components/ComponentArray.hpp"

class ComponentManager {
 public:
  template<typename T>
  void register_component();

  template<typename T>
  ComponentType get_component_type();

  template<typename T>
  void add_component(Entity entity, T component);

  template<typename T>
  void remove_component(Entity entity);

  template<typename T>
  T& get_component(Entity entity);

  void entity_destroyed(Entity entity);

 private:
  // Map from type string pointer to a component type
  std::unordered_map<const char*, ComponentType> m_component_types{};

  // Map from type string pointer to a component array
  std::unordered_map<const char*, std::shared_ptr<IComponentArray>> m_component_arrays{};

  // The component type to be assigned to the next registered component - starting at 0
  ComponentType mNextComponentType{};

  // Convenience function to get the statically casted pointer to the ComponentArray of type T.
  template<typename T>
  std::shared_ptr<ComponentArray<T>> get_component_array();
};

#endif  // SLIMEMAID_COMPONENTMANAGER_HPP
