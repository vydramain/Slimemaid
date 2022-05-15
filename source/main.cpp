/*
------------------------------------

Slimemaid Source Code (02.04.2022)

This file is part of Slimemaid Source Code.

This file has heading of allocation functions for Vulkan API.

------------------------------------
*/

#include <iostream>

#include "renderer/vulkan_instance_handler.hpp"

int main() {
  vulkan_instance_handler app;

  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
