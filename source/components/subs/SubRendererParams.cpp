/*
------------------------------------
  Slimemaid Source Code (18.05.2022)
  This file is part of Slimemaid Source Code.
------------------------------------
*/

#include "components/subs/SubRendererParams.hpp"

SubRendererParams::SubRendererParams() : AbsRendererParams() {
  WIDTH = 800;
  HEIGHT = 600;
  MAX_FRAMES_IN_FLIGHT = 2;
}

SubRendererParams::~SubRendererParams() = default;

void SubRendererParams::set_resolution(uint32_t input_width, uint32_t input_height) {
  WIDTH = input_width;
  HEIGHT = input_height;
}

const uint32_t* SubRendererParams::get_width() const { return &WIDTH; }
const uint32_t* SubRendererParams::get_height() const { return &HEIGHT; }
uint32_t SubRendererParams::get_max_frames_in_flight() const { return MAX_FRAMES_IN_FLIGHT; }