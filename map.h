#pragma once

#include <cstdint>
#include <optional>

#include "raylib.h"
#include "util.h"

#define MAP_BLOCK_WIDTH (WINDOW_BLOCK_WIDTH * 2)

struct Map {
  uint32_t map[MAP_BLOCK_WIDTH];
  int width;

  void draw(int scroll_offset);
  std::optional<int> next_floor(Rectangle p);
  std::optional<int> next_ceiling(Rectangle p);
  std::optional<int> next_left(Rectangle p);
  std::optional<int> next_right(Rectangle p);

  Map();
};
