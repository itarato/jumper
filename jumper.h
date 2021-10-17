#pragma once

#include "raylib.h"
#include "types.h"

struct Jumper {
  Rectangle frame;
  Vector2 v;

  void draw();

  Jumper();
};
