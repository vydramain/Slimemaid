/*
------------------------------------
  Slimemaid Source Code (20.05.2022)
  This file is part of Slimemaid Source Code.
------------------------------------
*/

#include "components/ComponentArray.hpp"

template<typename T>
void ComponentArray<T>::insert_data(Entity entity, T component) {
  assert(m_entity_to_index_map.find(entity) == m_entity_to_index_map.end() &&
         "Component added to same entity more than once.");

  // Put new entry at end and update the maps
  std::size_t newIndex = m_size;
  m_entity_to_index_map[entity] = newIndex;
  m_index_to_entity_map[newIndex] = entity;
  m_component_array[newIndex] = component;
  ++m_size;
}

template<typename T>
void ComponentArray<T>::remove_data(Entity entity) {
  assert(m_entity_to_index_map.find(entity) != m_entity_to_index_map.end() && "Removing non-existent component.");

  // Copy element at end into deleted element's place to maintain density
  std::size_t indexOfRemovedEntity = m_entity_to_index_map[entity];
  std::size_t indexOfLastElement = m_size - 1;
  m_component_array[indexOfRemovedEntity] = m_component_array[indexOfLastElement];

  // Update map to point to moved spot
  Entity entityOfLastElement = m_index_to_entity_map[indexOfLastElement];
  m_entity_to_index_map[entityOfLastElement] = indexOfRemovedEntity;
  m_index_to_entity_map[indexOfRemovedEntity] = entityOfLastElement;

  m_entity_to_index_map.erase(entity);
  m_index_to_entity_map.erase(indexOfLastElement);

  --m_size;
}

template<typename T>
T& ComponentArray<T>::get_data(Entity entity) {
  assert(m_entity_to_index_map.find(entity) != m_entity_to_index_map.end() && "Retrieving non-existent component.");

  // Return a reference to the entity's component
  return m_component_array[m_entity_to_index_map[entity]];
}

template<typename T>
void ComponentArray<T>::entity_destroyed(Entity entity) {
  if (m_entity_to_index_map.find(entity) != m_entity_to_index_map.end()) {
    // Remove the entity's component if it existed
    remove_data(entity);
  }
}
