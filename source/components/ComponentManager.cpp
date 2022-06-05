/*
------------------------------------
  Slimemaid Source Code (21.05.2022)
  This file is part of Slimemaid Source Code.
------------------------------------
*/

#include "components/ComponentManager.hpp"

template<typename T>
void ComponentManager::register_component() {
  const char* type_name = typeid(T).name();

  assert(m_component_types.find(type_name) == m_component_types.end() && "Registering component type more than once.");

  // Add this component type to the component type map
  m_component_types.insert({type_name, mNextComponentType});

  // Create a ComponentArray pointer and add it to the component arrays map
  m_component_arrays.insert({type_name, std::make_shared<ComponentArray<T>>()});

  // Increment the value so that the next component registered will be different
  ++mNextComponentType;
}

template<typename T>
ComponentType ComponentManager::get_component_type() {
  const char* type_name = typeid(T).name();

  assert(m_component_types.find(type_name) != m_component_types.end() && "Component not registered before use.");

  // Return this component's type - used for creating signatures
  return m_component_types[type_name];
}

template<typename T>
void ComponentManager::add_component(Entity entity, T component) {
  // Add a component to the array for an entity
  get_component_array<T>()->InsertData(entity, component);
}

template<typename T>
void ComponentManager::remove_component(Entity entity) {
  // Remove a component from the array for an entity
  get_component_array<T>()->RemoveData(entity);
}

template<typename T>
T& ComponentManager::get_component(Entity entity) {
  // Get a reference to a component from the array for an entity
  return get_component_array<T>()->GetData(entity);
}

void ComponentManager::entity_destroyed(Entity entity) {
  // Notify each component array that an entity has been destroyed
  // If it has a component for that entity, it will remove it
  for (auto const& pair : m_component_arrays) {
    auto const& component = pair.second;

    component->entity_destroyed(entity);
  }
}

template<typename T>
std::shared_ptr<ComponentArray<T>> ComponentManager::get_component_array() {
  const char* type_name = typeid(T).name();

  assert(m_component_types.find(type_name) != m_component_types.end() && "Component not registered before use.");

  return std::static_pointer_cast<ComponentArray<T>>(m_component_arrays[type_name]);
}
