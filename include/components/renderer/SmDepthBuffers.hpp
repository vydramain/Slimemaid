/*
------------------------------------
  Slimemaid Source Code (02.06.2022)
  This file is part of Slimemaid Source Code.
  Depth buffers solve problem of ordering fragments by depth.
  A depth buffer is an additional attachment that stores the depth for every position,
  just like the color attachment stores the color of every position. Every time the rasterizer produces a fragment,
  the depth test will check if the new fragment is closer than the previous one. If it isn't,
  then the new fragment is discarded. A fragment that passes the depth test writes its own depth to the depth buffer.
  It is possible to manipulate this value from the fragment shader, just like you can manipulate the color output.
------------------------------------
*/

#ifndef SLIMEMAID_SMDEPTHRESOURCES_HPP
#define SLIMEMAID_SMDEPTHRESOURCES_HPP

#include <vulkan/vulkan.h>

struct SmDepthBuffers {
  VkImage depthImage;
  VkDeviceMemory depthImageMemory;
  VkImageView depthImageView;
};

#endif  // SLIMEMAID_SMDEPTHRESOURCES_HPP
