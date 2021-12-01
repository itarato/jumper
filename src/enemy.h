#pragma once

#include <memory>

#include "raylib.h"
#include "shared/util.h"
#include "walker.h"

struct Enemy {
  Rectangle frame;
  std::unique_ptr<IWalker> walker;
  Phaser paralyzed_phaser{0.3f, 1.0f};
  OneTimeBool paralyzed;

  void init();
  void update(const Rectangle &player);
  void draw(IntVector2D scroll_offset);

  Enemy(Rectangle frame, std::unique_ptr<IWalker> walker);
};
