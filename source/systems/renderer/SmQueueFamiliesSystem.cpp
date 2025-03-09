
#include "systems/renderer/SmQueueFamiliesSystem.hpp"

#include <vector>

SmQueueFamilyIndices find_transfer_queue_families(VkPhysicalDevice input_device,
                                                  SmSurface input_surface) {
  SmQueueFamilyIndices indices;

  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(input_device,
                                           &queue_family_count,
                                           nullptr);

  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(input_device,
                                           &queue_family_count,
                                           queue_families.data());

  int i = 0;
  for (const auto& queue_family : queue_families) {
    if (indices.is_indices_complete()) {
      break;
    }
    if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphics_family = i;
    }
    VkBool32 present_support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(input_device,
                                         i,
                                         input_surface.surface_khr,
                                         &present_support);
    if (present_support) {
      indices.present_family = i;
    }

    i++;
  }

  return indices;
}
