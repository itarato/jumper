#pragma once

#include <string>
#include <vector>

struct Sprite {
  std::vector<std::string> img_names;
  int step;
  int counter;
  int frame_skip_count;

  Sprite(std::vector<std::string> img_names, int frame_skip_count);

  std::string current_img() const;
  void progress();
};
