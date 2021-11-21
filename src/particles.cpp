#include "particles.h"

#define PARTICLE_FADE_STEP 0.03f
#define PARTICLE_CIRCLER_ROT_STEP 0.1f

// EXPLOSION //////////////////////////////////////////////////////////////////

Explosion::Explosion(Rectangle start_frame, size_t particle_count, Color color) : particle_count(particle_count),
                                                                                  color(color) {
  for (size_t i = 0; i < particle_count; i++) {
    float angle = randf() * PI * 2.0f;
    float v = randf() * 2.0f + 5.0f;
    particle_v.emplace_back(sinf(angle) * v, cosf(angle) * v);
    particle_pos.emplace_back(start_frame.x + start_frame.width / 2, start_frame.y + start_frame.height / 2);
    particle_rot.emplace_back(randf() * PI * 2.0f);
  }
}

void Explosion::draw(IntVector2D scroll_offset) const {
  for (size_t i = 0; i < particle_count; i++) {
    Rectangle frame{particle_pos[i].x - scroll_offset.x, particle_pos[i].y - scroll_offset.y, 12.0f, 12.0f};
    DrawRectanglePro(frame, relative_midpoint(frame), particle_rot[i], color);
  }
}

void Explosion::update() {
  for (size_t i = 0; i < particle_count; i++) {
    particle_v[i].y += 0.4f;

    particle_pos[i].x += particle_v[i].x;
    particle_pos[i].y += particle_v[i].y;

    particle_v[i].x *= 0.92f;

    particle_rot[i] += particle_v[i].x >= 0.0f ? 1.0f : -1.0f;
  }

  fade -= PARTICLE_FADE_STEP;
}

// CIRCLER ////////////////////////////////////////////////////////////////////

Circler::Circler(Rectangle start_frame, size_t particle_count) : particle_count(particle_count),
                                                                 dist_step(4.0f) {
  for (size_t i = 0; i < particle_count; i++) {
    particle_pos.emplace_back(start_frame.x + start_frame.width / 2, start_frame.y + start_frame.height / 2);
    rot_offs.emplace_back(randf() * PI * 2.0f);
    dist_offs.emplace_back(randf() * 16.0f);
  }
}

void Circler::draw(IntVector2D scroll_offset) const {
  float toggle_rot;
  for (size_t i = 0; i < particle_count; i++) {
    toggle_rot = i % 2 == 0 ? rot : -rot;
    DrawRectangle(
            particle_pos[i].x - scroll_offset.x + (sinf(toggle_rot + rot_offs[i]) * (dist_offs[i] + dist)),
            particle_pos[i].y - scroll_offset.y + (cosf(toggle_rot + rot_offs[i]) * (dist_offs[i] + dist)),
            9,
            9,
            Fade(RED, fade));
  }
}

void Circler::update() {
  rot += PARTICLE_CIRCLER_ROT_STEP;
  dist += dist_step;
  dist_step *= 0.95f;
  fade *= 0.92f;
}

[[nodiscard]] bool Circler::is_completed() const {
  return dist >= 48.0f;
}

// SMOKER /////////////////////////////////////////////////////////////////////

Smoker::Smoker(Rectangle *start_frame, Color color) : start_frame(start_frame),
                                                      color(color) {}

void Smoker::draw(IntVector2D scroll_offset) const {
  for (int i = 0; i < (int) pos.size(); i++) {
    if (alpha[i] <= 0.0f) continue;

    DrawRectangle(pos[i].x - scroll_offset.x, pos[i].y - scroll_offset.y, 4, 4, Fade(color, alpha[i]));
  }
}

void Smoker::update() {
  if (start_frame == nullptr) return;

  for (int i = 0; i < (int) pos.size(); i++) {
    if (alpha[i] <= 0.0f) continue;

    pos[i].x += v[i].x;
    pos[i].y += v[i].y;
    alpha[i] -= 0.05;
  }

  if (total_frame_groups <= 0) return;

  if (frame_counter % round_frame_count == 0) {
    for (int i = 0; i < particle_per_round; i++) {
      pos.emplace_back(start_frame->x + randf() * start_frame->width, start_frame->y + start_frame->height);
      v.emplace_back(randf() / 2.0f, randf() - 0.8f);
      alpha.emplace_back(1.0f);
    }
  }
  frame_counter++;
  total_frame_groups--;
}

[[nodiscard]] bool Smoker::is_completed() const {
  return total_frame_groups <= 0 && alpha.back() <= 0.0f;
}

// REPEATER ///////////////////////////////////////////////////////////////////

Repeater::Repeater(
        Rectangle start_frame,
        int particle_per_round,
        int round_frame_count,
        int total_frame_groups,
        IParticle *(*particle_factory)(Rectangle)) : start_frame(start_frame),
                                                     particle_per_round(particle_per_round),
                                                     round_frame_count(round_frame_count),
                                                     total_frame_groups(total_frame_groups),
                                                     particle_factory(particle_factory) {}

Repeater::~Repeater() {
  for (auto &sub_particle : sub_particles) delete sub_particle;
  sub_particles.clear();
}

void Repeater::draw(IntVector2D scroll_offset) const {
  for (const auto &sub_particle : sub_particles) {
    if (sub_particle->is_completed()) continue;
    sub_particle->draw(scroll_offset);
  }
}

void Repeater::update() {
  if (total_frame_groups > 0) {

    if (frame_counter % round_frame_count == 0) {
      for (int i = 0; i < particle_per_round; i++) {
        sub_particles.push_back(particle_factory(start_frame));
      }
      total_frame_groups--;
      frame_counter = 0;
    }

    frame_counter++;
  }

  for (auto &sub_particle : sub_particles) {
    if (sub_particle->is_completed()) continue;
    sub_particle->update();
  }
}

bool Repeater::is_completed() const {
  return std::all_of(sub_particles.begin(), sub_particles.end(), [](const auto &sub_particle) { return sub_particle->is_completed(); });
}

// SPRINKLER //////////////////////////////////////////////////////////////////

Sprinkler::Sprinkler(Rectangle start_frame, float angle, uint64_t length, Color color) : ParticleFrameCapper(length),
                                                                                         color(color) {
  pos = absolute_midpoint(start_frame);

  angle += randf(-0.22f, 0.22f);
  v.x = cosf(angle) * 1.5f;
  v.y = sinf(angle) * 1.5f;

  mut_sum(pos, mul(v, 10.0f));

  if (fade > 0.0f) fade -= 0.2f;
}

void Sprinkler::draw(IntVector2D scroll_offset) const {
  DrawRectangle(pos.x - scroll_offset.x, pos.y - scroll_offset.y, 8, 8, Fade(color, fade));
}

void Sprinkler::update() {
  ParticleFrameCapper::tick();
  mut_sum(pos, v);
  mut_mul(v, 1.2f);
}

bool Sprinkler::is_completed() const {
  return ParticleFrameCapper::is_complete();
}
