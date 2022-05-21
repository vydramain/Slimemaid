/*
------------------------------------
  Slimemaid Source Code (20.05.2022)
  This file is part of Slimemaid Source Code.
  The Entity Manager is in charge of distributing entity IDs
  and keeping record of which IDs are in use and which are not.
------------------------------------
*/

#ifndef SLIMEMAID_ENTITYMANAGER_HPP
#define SLIMEMAID_ENTITYMANAGER_HPP

#include <array>
#include <cassert>
#include <queue>

#include "Signature.hpp"
#include "entities/Entity.hpp"

class EntityManager {
 public:
  EntityManager();

  Entity create_entity();
  void destroy_entity(Entity entity);
  void set_signature(Entity entity, Signature signature);
  Signature get_signature(Entity entity);

 private:
  // Queue of unused entity IDs
  std::queue<Entity> m_available_entities{};

  // Array of signatures where the index corresponds to the entity ID
  std::array<Signature, MAX_ENTITIES> m_signatures{};

  // Total living entities - used to keep limits on how many exist
  uint32_t m_living_entity_count{};
};

#endif  // SLIMEMAID_ENTITYMANAGER_HPP
