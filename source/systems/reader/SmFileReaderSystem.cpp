/*
------------------------------------
 Slimemaid Source Code (15.06.2022)
 This file is part of Slimemaid Source Code.
----------------------------------
*/

#include "systems/reader/SmFileReaderSystem.hpp"

std::vector<char> read_file(const std::string& filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file: " + filename);
  }

  size_t file_size = (size_t) file.tellg();
  std::vector<char> buffer(file_size);

  file.seekg(0);
  file.read(buffer.data(), file_size);
  file.close();
  return buffer;
}

