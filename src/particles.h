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
  std::vector<float> particle_rot;

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
  float dist_step;
  float fade = 1.0f;

  Circler(Rectangle start_frame, size_t particle_count);

  void draw(IntVector2D scroll_offset) const override;
  void update() override;
  [[nodiscard]] bool is_completed() const override;
};

struct Smoker : IParticle {
  Rectangle start_frame;
  int particle_per_round{1};// How many new particles per frame group.
  int round_frame_count{1}; // How many frame is in one frame group.
  int frame_counter{0};
  int total_frame_groups{16};// How many frame groups to do.
  std::vector<Vector2> pos{};
  std::vector<Vector2> v{};
  std::vector<float> alpha{};

  Smoker(Rectangle start_frame);

  void draw(IntVector2D scroll_offset) const override;
  void update() override;
  [[nodiscard]] bool is_completed() const override;
};
