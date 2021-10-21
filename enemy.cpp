#include "enemy.h"

#include "util.h"

void Enemy::init() {
  frame = Rectangle{
      (float)BLOCK_SIZE * 13.0f,
      (float)BLOCK_SIZE * 9.0f,
      (float)BLOCK_SIZE,
      (float)BLOCK_SIZE,
  };
  origin = Vector2{(float)BLOCK_SIZE * 13.0f, (float)BLOCK_SIZE * 9.0f};
  target = Vector2{(float)BLOCK_SIZE * 13.0f, (float)BLOCK_SIZE * 8.0f};
}

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

void Enemy::set_next_target() {
  step = 0;
  origin = target;

  switch (rand_range(0, 3)) {
    case 0:
      target.x += BLOCK_SIZE;
      break;
    case 1:
      target.x -= BLOCK_SIZE;
      break;
    case 2:
      target.y += BLOCK_SIZE;
      break;
    case 3:
      target.y -= BLOCK_SIZE;
      break;
  }
}

void Enemy::draw(int draw_offset) { DrawRectangleRec(frame, BLACK); }
