#pragma once

#include <string>
#include <vector>

#include "raylib.h"

#define SPRITE_CYCLE true
#define SPRITE_NO_CYCLE false

struct Sprite {
  std::vector<Texture2D*> textures{};
  int step{0};
  int counter{0};
  // How much a frames a single sprite-step last. The higher the number the
  // slower the sprite animation.
  int frame_skip_count{4};
  int should_cycle{true};

  Sprite(const char* img_names_format, int frame_skip_count, bool should_cycle);
  Sprite(const char* img_names_format);

  [[nodiscard]] Texture2D* texture() const;

  void progress();
  void regress();

  bool is_progress_completed() const;
  bool is_regress_completed() const;

 private:
  bool is_ready_to_step() const { return counter > frame_skip_count; }
};
