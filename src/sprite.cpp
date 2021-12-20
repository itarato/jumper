#include "sprite.h"

#include <utility>

#include "asset_manager.h"

Sprite::Sprite(std::vector<std::string> img_names, int frame_skip_count)
    : img_names(move(img_names)), frame_skip_count(frame_skip_count) {}

Sprite::Sprite(const char* img_names_format, int frame_skip_count)
    : frame_skip_count(frame_skip_count) {
  img_names = asset_manager.texture_name_list(img_names_format);
}

std::string Sprite::current_img() const { return img_names[step]; }

void Sprite::progress() {
  counter++;

  if (counter >= frame_skip_count) {
    step++;
    counter = 0;
  }

  if (step >= (int)img_names.size()) step = 0;
}
