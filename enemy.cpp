#include "enemy.h"

bool Enemy::is_target_reached() {
  // Be aware of float comparison problems.
  return frame.x == target.x && frame.y == target.y;
}

void Enemy::update() {
  if (step >= ENEMY_STEP_COUNT) {
    // return;
    set_next_target();
  }

  step++;

  float fragment_multiplier = (float)step / (float)ENEMY_STEP_COUNT;
  frame.x = origin.x + ((target.x - origin.x) * fragment_multiplier);
  frame.y = origin.y + ((target.y - origin.y) * fragment_multiplier);
}

void Enemy::set_next_target() { step = 0; }
