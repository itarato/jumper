#pragma once

#include "shared/types.h"

// Idea: poop could be as strong as the struggle -- so tiny poops are quicker
// but less potent.

struct Poop : Killable {
  int timer{360};
  Vector2 pos;

  explicit Poop(Vector2 pos);

  void update();
  void draw(IntVector2D screen_offset) const;

  [[nodiscard]] Rectangle frame() const;
};
