#pragma once

#include "raylib.h"
#include "walker.h"

struct Enemy {
  Rectangle frame;

  IWalker *walker;

  void init();
  void update(const Rectangle &player);
  void draw(int scroll_offset);

  Enemy(Map *map);
  ~Enemy();

 private:
  void set_next_target();
};
