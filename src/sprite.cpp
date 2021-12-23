#include "sprite.h"

#include <utility>

#include "asset_manager.h"

Sprite::Sprite(const char* img_names_format, int frame_skip_count,
               bool should_cycle)
    : textures(asset_manager.texture_list(img_names_format)),
      frame_skip_count(frame_skip_count),
      should_cycle(should_cycle) {
  LOG_INFO("Loading sprite %s, found %d steps.", img_names_format,
           (int)textures.size());

  if (textures.size() == 0) {
    PANIC("Failed loading sprite: %s\n", img_names_format);
  }
}

Sprite::Sprite(const char* img_names_format)
    : Sprite(img_names_format, 4, SPRITE_CYCLE) {}

Texture2D* Sprite::texture() const {
  if (step >= (int)textures.size()) {
    PANIC("Sprite step out of bounds. Size: %d Step: %d\n",
          (int)textures.size(), step);
  }
  return textures[step];
}

bool Sprite::is_progress_completed() const {
  if (should_cycle) return false;

  return step == (int)textures.size() - 1 && is_ready_to_step();
}

bool Sprite::is_regress_completed() const {
  if (should_cycle) return false;

  return step == 0 && is_ready_to_step();
}

void Sprite::progress() {
  if (is_progress_completed()) {
    return;
  } else {
    counter++;
  }

  if (is_ready_to_step()) {
    step++;

    if (step >= (int)textures.size()) {
      if (should_cycle) {
        step = 0;
        counter = 0;
      } else {
        step = (int)textures.size() - 1;
      }
    } else {
      counter = 0;
    }
  }
}

void Sprite::regress() {
  if (is_regress_completed()) {
    return;
  } else {
    counter++;
  }

  if (is_ready_to_step()) {
    step--;

    if (step < 0) {
      if (should_cycle) {
        step = (int)textures.size() - 1;
        counter = 0;
      } else {
        step = 0;
      }
    } else {
      counter = 0;
    }
  }
}
