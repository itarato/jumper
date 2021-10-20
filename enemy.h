#pragma once

#include "raylib.h"

#define ENEMY_STEP_COUNT 8

struct Enemy {
  Rectangle frame;
  Vector2 origin;
  Vector2 target;
  int step;

  bool is_target_reached();
  void update();
  void set_next_target();

  Enemy() {}
};
