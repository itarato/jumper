#pragma once

#include <vector>

#include "raylib.h"
#include "shared/types.h"

struct Explosion {
  size_t particle_count;
  int particle_size;

  std::vector<Vector2> particle_vs;

  Explosion(size_t particle_count, int particle_size = 4) : particle_count(particle_count),
                                                            particle_size(particle_size) {
  }

  void draw(IntVector2D scroll_offset) const;
  void update();
};
