#pragma once

#include "util.h"
#include <cstdint>

struct Map {
  uint32_t map[WINDOW_BLOCK_WIDTH];

  void draw();
  int next_floor_from(int y);
  int next_ceiling_from(int y);

  Map();
};
