#pragma once

#include <cmath>
#include <cstdlib>
#include <vector>

#include "raylib.h"
#include "shared/types.h"
#include "shared/util.h"

struct IParticle {
  virtual void draw(IntVector2D scroll_offset) const = 0;
  virtual void update() = 0;
  [[nodiscard]] virtual bool is_completed() const = 0;
  virtual ~IParticle() = default;
};

struct Explosion : IParticle {
  size_t particle_count;
  float fade = 1.0f;
  std::vector<Vector2> particle_v;
  std::vector<Vector2> particle_pos;

  Explosion(Rectangle start_frame, size_t particle_count);

  void draw(IntVector2D scroll_offset) const override;
  void update() override;
  [[nodiscard]] bool is_completed() const override { return fade < 0.0f; }
};

struct Circler : IParticle {
  size_t particle_count;
  std::vector<Vector2> particle_pos{};
  std::vector<float> rot_offs{};
  std::vector<float> dist_offs{};
  float rot = 0.0f;
  float dist = 0.0f;
  float dist_step = 4.0f;
  float fade = 1.0f;

  Circler(Rectangle start_frame, size_t particle_count);

  void draw(IntVector2D scroll_offset) const override;
  void update() override;
  [[nodiscard]] bool is_completed() const override;
};
