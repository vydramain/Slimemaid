/*
------------------------------------
  Slimemaid Source Code (07.06.2022)
  This file is part of Slimemaid Source Code.
  Vulkan instance system create Vulkan instance. Just create. Deletion should be controlled explicitly.
------------------------------------
*/

#ifndef SLIMEMAID_SMVULKANINSTANCESYSTEM_HPP
#define SLIMEMAID_SMVULKANINSTANCESYSTEM_HPP

#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>

#include "systems/renderer/SmGLFWWindowSystem.hpp"
#include "systems/debug/SmDebugSystem.hpp"

#include "components/renderer/SmVulkanInstance.hpp"

void create_instance(SmVulkanInstance* p_instance,
                     bool input_enable_validation_layers);

#endif  // SLIMEMAID_SMVULKANINSTANCESYSTEM_HPP
