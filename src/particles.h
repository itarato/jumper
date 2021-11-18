#pragma once

#include <algorithm>
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
  int frame_counter{0};
  int total_frame_groups{4};// How many frame groups to do.

  IParticle *(*particle_factory)(Rectangle start_frame);

  std::vector<IParticle *> sub_particles;

  Repeater(Rectangle start_frame, IParticle *(*particle_factory)(Rectangle start_frame)) : start_frame(start_frame),
                                                                                           particle_factory(particle_factory) {}

  ~Repeater() override {
    for (auto &sub_particle : sub_particles) {
      delete sub_particle;
    }
    sub_particles.clear();
  }

  void draw(IntVector2D scroll_offset) const override {
    for (const auto &sub_particle : sub_particles) {
      sub_particle->draw(scroll_offset);
    }
  }

  void update() override {
    if (total_frame_groups > 0) {

      if (frame_counter % round_frame_count == 0) {
        for (int i = 0; i < particle_per_round; i++) {
          sub_particles.push_back(particle_factory(start_frame));
        }
      }
      frame_counter++;
      total_frame_groups--;
    }

    for (auto &sub_particle : sub_particles) {
      if (!sub_particle->is_completed()) {
        sub_particle->update();
      }
    }
  }

  [[nodiscard]] bool is_completed() const override {
    bool done = std::all_of(sub_particles.begin(), sub_particles.end(), [](const auto &sub_particle) { return sub_particle->is_completed(); });

    for (int i = 0; i < sub_particles.size(); i++) {
      if (!sub_particles[i]->is_completed()) {
        LOG_INFO("NOT COMPLETED: %d", i);
      }
    }

    return done;
  }
};
