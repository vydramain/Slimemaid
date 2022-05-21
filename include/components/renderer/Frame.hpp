/*
------------------------------------
  Slimemaid Source Code (18.05.2022)
  This file is part of Slimemaid Source Code.
------------------------------------
*/

#ifndef SLIMEMAID_ABSRENDERERPARAMS_HPP
#define SLIMEMAID_ABSRENDERERPARAMS_HPP

#include <cstdint> // Necessary for uint32_t

struct Frame {
   uint32_t WIDTH = 800;
   uint32_t HEIGHT = 600;
   uint32_t MAX_FRAMES_IN_FLIGHT = 2;
};

#endif  // SLIMEMAID_ABSRENDERERPARAMS_HPP
