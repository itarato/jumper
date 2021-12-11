#pragma once

#include "raylib.h"
#include "shared/types.h"

struct Collectible {
  Rectangle frame;
  bool is_collected;

  explicit Collectible(Vector2 pos);

  virtual void draw(IntVector2D screen_offset) const = 0;
};

struct Coin : Collectible {
  explicit Coin(Vector2 pos) : Collectible(pos) {}
  void draw(IntVector2D screen_offset) const override;
};

struct Shield : Collectible {
  explicit Shield(Vector2 pos) : Collectible(pos) {}
  void draw(IntVector2D screen_offset) const override;
};
