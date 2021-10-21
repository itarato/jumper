#include "walker.h"

#include "util.h"

void RandomWalker::init(Rectangle &frame) {
  frame = Rectangle{
      (float)BLOCK_SIZE * 13.0f,
      (float)BLOCK_SIZE * 9.0f,
      (float)BLOCK_SIZE,
      (float)BLOCK_SIZE,
  };
  origin = Vector2{(float)BLOCK_SIZE * 13.0f, (float)BLOCK_SIZE * 9.0f};
  target = Vector2{(float)BLOCK_SIZE * 13.0f, (float)BLOCK_SIZE * 8.0f};
}

void RandomWalker::update(Rectangle &frame) {
  if (step >= RANDOM_WALKER_STEP_COUNT) {
    set_next_target();
  }

  step++;

  float fragment_multiplier = (float)step / (float)RANDOM_WALKER_STEP_COUNT;
  frame.x = origin.x + ((target.x - origin.x) * fragment_multiplier);
  frame.y = origin.y + ((target.y - origin.y) * fragment_multiplier);
}

void RandomWalker::set_next_target() {
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