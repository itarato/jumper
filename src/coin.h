#pragma once

#include "raylib.h"

struct Coin {
  Rectangle frame;
  bool is_collected;

  explicit Coin(Vector2 pos);

  void draw(int screen_offset) const;
};
