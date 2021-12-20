#include "sprite.h"

#include <utility>

#include "asset_manager.h"

Sprite::Sprite(const char* img_names_format, int frame_skip_count)
    : frame_skip_count(frame_skip_count),
      textures(asset_manager.texture_list(img_names_format)) {
  LOG_INFO("Loading sprite %s, found %d steps.", img_names_format,
           (int)textures.size());

  if (textures.size() == 0) {
    PANIC("Failed loading sprite: %s\n", img_names_format);
  }
}

Texture2D* Sprite::current_img() const {
  if (step >= textures.size()) {
    PANIC("Sprite step out of bounds. Size: %d Step: %d\n",
          (int)textures.size(), step);
  }
  return textures[step];
}

void Sprite::progress() {
  counter++;

  if (counter >= frame_skip_count) {
    step++;
    counter = 0;
  }

  if (step >= (int)textures.size()) step = 0;
}
