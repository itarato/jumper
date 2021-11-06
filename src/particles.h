#pragma once

#include <cmath>
#include <cstdlib>
#include <vector>

#include "raylib.h"
#include "shared/types.h"
#include "shared/util.h"

struct Explosion {
  size_t particle_count;
  float fade = 1.0f;

  std::vector<Vector2> particle_v;
  std::vector<Vector2> particle_pos;

  Explosion(Rectangle start_frame, size_t particle_count) : particle_count(particle_count) {
    for (size_t i = 0; i < particle_count; i++) {
      float angle = randf() * PI * 2.0f;
      float v = randf() * 10.0f;
      particle_v.emplace_back(sinf(angle) * v, cosf(angle) * v);
      particle_pos.emplace_back(start_frame.x + start_frame.width / 2, start_frame.y + start_frame.height / 2);
    }
  }

  void draw(IntVector2D scroll_offset) const;
  void update();
  [[nodiscard]] bool is_completed() const { return fade < 0.0f; }
};
