#pragma once

#include "shared/types.h"

// Idea: poop could be as strong as the struggle -- so tiny poops are quicker but less potent.

struct Poop {
  int timer{120};
  Vector2 pos;

  Poop(Vector2 pos);

  void update();
  void draw(IntVector2D screen_offset) const;

  bool is_dead() const;
  Rectangle frame() const;
};
