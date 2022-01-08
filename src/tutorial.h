#pragma once

#include <string>

#include "raylib.h"
#include "shared/util.h"

struct Tutorial : Killable {
  std::string text{};
  Rectangle block_frame{};
  Rectangle text_block_frame{};

  Tutorial(std::string text, IntVector2D tile_coords);

  void draw(IntVector2D scroll_offset) const;
};
