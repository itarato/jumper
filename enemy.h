#pragma once

#include "raylib.h"
#include "walker.h"

struct Enemy {
  Rectangle frame;

  IWalker *walker;

  void init();
  void update();
  void set_next_target();
  void draw(int scroll_offset);

  Enemy(Map *map);
  ~Enemy();
};
