#include "particles.h"

#define PARTICLE_FADE_STEP 0.04f
#define PARTICLE_CIRCLER_ROT_STEP 0.1f

// EXPLOSION //////////////////////////////////////////////////////////////////

Explosion::Explosion(Rectangle start_frame, size_t particle_count) : particle_count(particle_count) {
  for (size_t i = 0; i < particle_count; i++) {
    float angle = randf() * PI * 2.0f;
    float v = randf() * 2.0f + 5.0f;
    particle_v.emplace_back(sinf(angle) * v, cosf(angle) * v);
    particle_pos.emplace_back(start_frame.x + start_frame.width / 2, start_frame.y + start_frame.height / 2);
  }
}

void Explosion::draw(IntVector2D scroll_offset) const {
  for (size_t i = 0; i < particle_count; i++) {
    DrawRectangle(particle_pos[i].x - scroll_offset.x, particle_pos[i].y - scroll_offset.y, 12, 12, Fade(DARKGRAY, fade));
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

// CIRCLER ////////////////////////////////////////////////////////////////////

Circler::Circler(Rectangle start_frame, size_t particle_count) : particle_count(particle_count) {
  for (size_t i = 0; i < particle_count; i++) {
    particle_pos.emplace_back(start_frame.x + start_frame.width / 2, start_frame.y + start_frame.height / 2);
    rot_offs.emplace_back(randf() * PI * 2.0f);
    dist_offs.emplace_back(randf() * 20.0f + 20.0f);
  }
}

void Circler::draw(IntVector2D scroll_offset) const {
  float toggle_rot;
  for (size_t i = 0; i < particle_count; i++) {
    toggle_rot = i % 2 == 0 ? rot : -rot;
    DrawRectangle(
            particle_pos[i].x - scroll_offset.x + (sinf(toggle_rot + rot_offs[i]) * (dist_offs[i] + dist)),
            particle_pos[i].y - scroll_offset.y + (cosf(toggle_rot + rot_offs[i]) * (dist_offs[i] + dist)),
            3,
            3,
            Fade(RED, fade));
  }
}

void Circler::update() {
  rot += PARTICLE_CIRCLER_ROT_STEP;
  dist += dist_step;
  dist_step /= 1.1f;
  fade *= 0.95f;
}

[[nodiscard]] bool Circler::is_completed() const {
  return rot >= PI / 2.0f;
}
