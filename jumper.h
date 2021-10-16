#pragma once

#include "raylib.h"
#include "types.h"

struct Jumper : public IMovable {
  Vector2 pos;
  Vector2 v;
  Vector2 bounds;

  void draw();

  Vector2 get_pos() const;
  Vector2 get_v() const;
  void move(Vector2 delta);

  Jumper();
};
