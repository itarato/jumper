#pragma once

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <vector>

#include "raylib.h"
#include "shared/types.h"
#include "shared/util.h"

struct ParticleFrameCapper {
  uint64_t frame_counter{0};
  uint64_t cap;

  ParticleFrameCapper(uint64_t cap) : cap(cap) {}

  void tick() {
    frame_counter++;
  }

  [[nodiscard]] bool is_complete() const {
    return frame_counter >= cap;
  }
};

struct IParticle {
  virtual void draw(IntVector2D scroll_offset) const = 0;
  virtual void update() = 0;
  [[nodiscard]] virtual bool is_completed() const = 0;
  virtual ~IParticle() = default;
};

struct Explosion : IParticle {
  size_t particle_count;
  float fade = 1.0f;
  Color color;
  std::vector<Vector2> particle_v;
  std::vector<Vector2> particle_pos;
  std::vector<float> particle_rot;

  Explosion(Rectangle start_frame, size_t particle_count, Color color);

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
  Rectangle *start_frame;
  int particle_per_round{1};// How many new particles per frame group.
  int round_frame_count{1}; // How many frame is in one frame group.
  int frame_counter{0};
  int total_frame_groups{16};// How many frame groups to do.
  std::vector<Vector2> pos{};
  std::vector<Vector2> v{};
  std::vector<float> alpha{};
  Color color;

  Smoker(Rectangle *start_frame, Color color);

  void draw(IntVector2D scroll_offset) const override;
  void update() override;
  [[nodiscard]] bool is_completed() const override;
};

struct Repeater : IParticle {
  Rectangle start_frame;

  int particle_per_round{1};// How many new particles per frame group.
  int round_frame_count{1}; // How many frame is in one frame group.
  int total_frame_groups{4};// How many frame groups to do.

  int frame_counter{0};

  IParticle *(*particle_factory)(Rectangle start_frame);

  std::vector<IParticle *> sub_particles;

  Repeater(
          Rectangle start_frame,
          int particle_per_round,
          int round_frame_count,
          int total_frame_groups,
          IParticle *(*particle_factory)(Rectangle));

  ~Repeater() override;

  void draw(IntVector2D scroll_offset) const override;
  void update() override;
  [[nodiscard]] bool is_completed() const override;
};

struct Sprinkler : IParticle, ParticleFrameCapper {
  Vector2 pos;
  Vector2 v;

  Sprinkler(Rectangle start_frame, float angle, uint64_t length);

  void draw(IntVector2D scroll_offset) const override;
  void update() override;
  [[nodiscard]] bool is_completed() const override;
};
