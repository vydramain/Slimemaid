/*
------------------------------------
  Slimemaid Source Code (21.05.2022)
  This file is part of Slimemaid Source Code.
  The System Manager is in charge of maintaining a record of registered systems and their signatures.
------------------------------------
*/

#ifndef SLIMEMAID_SYSTEMMANAGER_HPP
#define SLIMEMAID_SYSTEMMANAGER_HPP

#include <cassert>
#include <memory>
#include <unordered_map>

#include "entities/Entity.hpp"
#include "entities/Signature.hpp"

#include "systems/System.hpp"

class SystemManagerSystem {
 public:
  template<typename T>
  std::shared_ptr<T> register_system();

  template<typename T>
  void set_signature(Signature signature);

  void entity_destroyed(Entity entity);

  void entity_signature_changed(Entity entity, Signature entity_signature);

 private:
  // Map from system type string pointer to a signature
  std::unordered_map<const char*, Signature> m_signatures{};

  // Map from system type string pointer to a system pointer
  std::unordered_map<const char*, std::shared_ptr<System>> m_systems{};
};

#endif  // SLIMEMAID_SYSTEMMANAGER_HPP
