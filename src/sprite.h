#pragma once

#include <string>
#include <vector>

#include "raylib.h"

struct Sprite {
  std::vector<Texture2D*> textures{};
  int step{0};
  int counter{0};
  // How much a frames a single sprite-step last. The higher the number the
  // slower the sprite animation.
  int frame_skip_count{4};

  Sprite(const char* img_names_format, int frame_skip_count);

  [[nodiscard]] Texture2D* current_img() const;
  void progress();
};
