#pragma once

#include "raylib.h"
#include "types.h"

struct Jumper {
  Vector2 pos;
  Vector2 v;
  Vector2 bounds;

  void draw();
  Rectangle frame();

  Jumper();
};
