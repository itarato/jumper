#pragma once

#include <cstdint>
#include <optional>

#include "raylib.h"
#include "util.h"

struct Map {
  uint32_t map[WINDOW_BLOCK_WIDTH * 2];
  int width;

  void draw(int scroll_offset);
  std::optional<int> next_floor(Rectangle p);
  std::optional<int> next_ceiling(Rectangle p);

  Map();
};
