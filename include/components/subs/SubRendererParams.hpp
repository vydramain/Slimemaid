/*
------------------------------------
  Slimemaid Source Code (18.05.2022)
  This file is part of Slimemaid Source Code.
------------------------------------
*/

#ifndef SLIMEMAID_SUBRENDERERPARAMS_HPP
#define SLIMEMAID_SUBRENDERERPARAMS_HPP

#include <cstdint> // Necessary for uint32_t

#include "components/abs/AbsRendererParams.hpp"

class SubRendererParams : public AbsRendererParams {
 public:
    explicit SubRendererParams();
    virtual  ~SubRendererParams();

    void set_resolution(uint32_t input_width, uint32_t input_height);

    [[nodiscard]] const uint32_t* get_width() const;
    [[nodiscard]] const uint32_t* get_height() const;
    [[nodiscard]] uint32_t get_max_frames_in_flight() const;
};

#endif  // SLIMEMAID_SUBRENDERERPARAMS_HPP
