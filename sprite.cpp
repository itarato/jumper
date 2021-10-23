#include "sprite.h"

#include <utility>

Sprite::Sprite(std::vector<std::string> img_names)
    : img_names(move(img_names)) {}

Sprite::Sprite(std::vector<std::string> img_names, int frame_skip_count)
    : img_names(move(img_names)), frame_skip_count(frame_skip_count) {}

std::string Sprite::current_img() const { return img_names[step]; }

void Sprite::progress() {
  counter++;

  if (counter >= frame_skip_count) {
    step++;
    counter = 0;
  }

  if (step >= (int)img_names.size()) {
    step = 0;
  }
}
