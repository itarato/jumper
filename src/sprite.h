#pragma once

#include <string>
#include <vector>

struct Sprite {
  std::vector<std::string> img_names;
  int step{0};
  int counter{0};
  int frame_skip_count{0};

  Sprite(std::vector<std::string> img_names, int frame_skip_count);

  [[nodiscard]] std::string current_img() const;
  void progress();
};
