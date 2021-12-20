#pragma once

#include <string>
#include <vector>

struct Sprite {
  std::vector<std::string> img_names;
  int step{0};
  int counter{0};
  // How much a frames a single sprite-step last. The higher the number the
  // slower the sprite animation.
  int frame_skip_count{4};

  // TODO: this is a smell. Should we just only provide texture accessors?
  Sprite(std::vector<std::string> img_names, int frame_skip_count);
  Sprite(const char* img_names_format, int frame_skip_count);

  [[nodiscard]] std::string current_img() const;
  void progress();
};
