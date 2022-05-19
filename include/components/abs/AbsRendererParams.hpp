/*
------------------------------------
  Slimemaid Source Code (18.05.2022)
  This file is part of Slimemaid Source Code.
------------------------------------
*/

#ifndef SLIMEMAID_ABSRENDERERPARAMS_HPP
#define SLIMEMAID_ABSRENDERERPARAMS_HPP

#include <cstdint> // Necessary for uint32_t

class AbsRendererParams {
  protected:
   uint32_t WIDTH;
   uint32_t HEIGHT;
   uint32_t MAX_FRAMES_IN_FLIGHT;
};

#endif  // SLIMEMAID_ABSRENDERERPARAMS_HPP
