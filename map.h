#pragma once

#include "raylib.h"
#include "util.h"
#include <cstdint>
#include <optional>

struct Map {
  uint32_t map[WINDOW_BLOCK_WIDTH];

  void draw();
  std::optional<int> next_floor(Rectangle p);
  std::optional<int> next_ceiling(Rectangle p);

  Map();
};
