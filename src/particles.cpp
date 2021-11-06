#include "particles.h"

#define PARTICLE_FADE_STEP 0.02f

void Explosion::draw(IntVector2D scroll_offset) const {
  for (size_t i = 0; i < particle_count; i++) {
    DrawRectangle(particle_pos[i].x - scroll_offset.x, particle_pos[i].y - scroll_offset.y, 3, 3, Fade(DARKGRAY, fade));
  }
}

void Explosion::update() {
  for (size_t i = 0; i < particle_count; i++) {
    particle_v[i].y += 0.4f;

    particle_pos[i].x += particle_v[i].x;
    particle_pos[i].y += particle_v[i].y;
  }

  fade -= PARTICLE_FADE_STEP;
}
