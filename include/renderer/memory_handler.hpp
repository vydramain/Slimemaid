/*
------------------------------------

Slimemaid Source Code (08.05.2022)

This file is part of Slimemaid Source Code.

This file has heading of allocation functions for Vulkan API.

------------------------------------
*/

#ifndef SM_VULKAN_MEMORY_HANDLER_
#define SM_VULKAN_MEMORY_HANDLER_

#include <vulkan/vulkan.h>

#include <cstdint>              // Necessary for uint32_t
#include <stdexcept>            // Necessary for runtime_error

/*
------------
Function for determining the type of memory.
This function gets physical device and type filter as unsigned int and mempry properties.
Memory types are determined depending by physical device featrues and type filter requirements.
------------
*/
uint32_t findMemoryType(VkPhysicalDevice& inputPhysicalDevice,
                        uint32_t inputTypeFilter,
                        VkMemoryPropertyFlags inputProperties);
                        
#endif

