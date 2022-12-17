/*
------------------------------------
  Slimemaid Source Code (21.05.2022)
  This file is part of Slimemaid Source Code.
  The System Manager is in charge of maintaining a record of registered systems and their signatures.
------------------------------------
*/

#include "systems/SystemManager.hpp"

template<typename T>
std::shared_ptr<T> SystemManagerSystem::register_system() {
  const char* type_name = typeid(T).name();                          

  assert(m_systems.find(type_name) == m_systems.end() && "Registering system more than once.");

  // Create a pointer to the system and return it so it can be used externally
  auto system = std::make_shared<T>();
  m_systems.insert({type_name, system});
  return system;
}

template<typename T>
void SystemManagerSystem::set_signature(Signature signature) {
  const char* type_name = typeid(T).name();

  assert(m_systems.find(type_name) != m_systems.end() && "System used before registered.");

  // Set the signature for this system
  m_signatures.insert({type_name, signature});
}

void SystemManagerSystem::entity_destroyed(Entity entity) {
  // Erase a destroyed entity from all system lists
  // m_entities is a set so no check needed
  for (auto const& pair : m_systems) {
    auto const& system = pair.second;

    system->m_entities.erase(entity);
  }
}

void SystemManagerSystem::entity_signature_changed(Entity entity, Signature entity_signature) {
  // Notify each system that an entity's signature changed
  for (auto const& pair : m_systems) {
    auto const& type = pair.first;
    auto const& system = pair.second;
    auto const& system_signature = m_signatures[type];

    // Entity signature matches system signature - insert into set
    if ((entity_signature & system_signature) == system_signature) {
      system->m_entities.insert(entity);
    }
    // Entity signature does not match system signature - erase from set
    else {
      system->m_entities.erase(entity);
    }
  }
}
