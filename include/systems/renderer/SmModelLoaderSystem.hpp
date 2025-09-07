/*
------------------------------------
  Slimemaid Source Code (22.05.2022)
  This file is part of Slimemaid Source Code.
  Model loader system provide function for load models to special model resources object.
------------------------------------
*/

#ifndef SLIMEMAID_MODELLOADER_HPP
#define SLIMEMAID_MODELLOADER_HPP

#include "tiny_obj_loader.h"

#include "components/renderer/SmModelResources.hpp"

const std::string MODEL_PATH = "raws/viking_room/viking_room.obj";
const std::string TEXTURE_PATH = "raws/viking_room/viking_room.png";

// const std::string MODEL_PATH = "raws/toilet/source/tub.obj";
// const std::string TEXTURE_PATH = "raws/toilet/textures/Default_Base_color.png";

void sl_load_model(SmModelResources* p_model_resources);

#endif  // SLIMEMAID_MODELLOADER_HPP
