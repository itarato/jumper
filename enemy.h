#pragma once

#include "raylib.h"

#define ENEMY_STEP_COUNT 32

struct Enemy {
  Rectangle frame;
  Vector2 origin;
  Vector2 target;
  int step;

  void init();
  bool is_target_reached();
  void update();
  void set_next_target();
  void draw(int scroll_offset);

  Enemy() {}
};
