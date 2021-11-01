#pragma once

#include "raylib.h"
#include "shared/types.h"

struct Coin {
  Rectangle frame;
  bool is_collected;

  explicit Coin(Vector2 pos);

  void draw(IntVector2D screen_offset) const;
};
