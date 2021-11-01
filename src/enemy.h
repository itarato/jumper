#pragma once

#include <memory>

#include "raylib.h"
#include "walker.h"

struct Enemy {
  Rectangle frame;
  std::unique_ptr<IWalker> walker;

  void init();
  void update(const Rectangle &player);
  void draw(IntVector2D scroll_offset) const;

  Enemy(Rectangle frame, std::unique_ptr<IWalker> walker);
};
