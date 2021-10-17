#include "stage_game.h"

#include <algorithm>
#include <cmath>

#include "util.h"

#define JUMPER_HMOVE_V 5.0f
#define JUMP_FORCE 16.0f

#define WINDOW_SCROLL_PADDING 256

struct DoubleJump {
  Map *map;
  Jumper *jumper;

  bool can_jump() {}
};

void StageGame::update() {
  { // Horizontal movement.
    if (IsKeyDown(KEY_LEFT)) {
      jumper.v.x = -JUMPER_HMOVE_V;
    } else if (IsKeyDown(KEY_RIGHT)) {
      jumper.v.x = JUMPER_HMOVE_V;
    } else {
      jumper.v.x = 0.0f;
    }

    if (jumper.v.x < 0.0f) { // Going left.
      float left_wall_x = map.next_left(jumper.frame).value_or(0);
      jumper.frame.x = std::max(jumper.frame.x + jumper.v.x, left_wall_x);
    } else if (jumper.v.x > 0.0f) { // Going right.
      float right_wall_x =
          map.next_right(jumper.frame).value_or(map.width) - jumper.frame.width;
      LOG_INFO("Jumperx: %.1f Right: %.2f", jumper.frame.x + jumper.frame.width,
               right_wall_x);
      jumper.frame.x = std::min(jumper.frame.x + jumper.v.x, right_wall_x);
    }
  }

  { // Vertical movement.
    if (IsKeyPressed(KEY_SPACE)) {
      jumper.v.y -= JUMP_FORCE;
    }
  }

  { // Vertical movement.
    map.evaluate_map_object_state(&jumper);

    switch (jumper.map_state.type) {
    case MAP_OBJECT_STATE_TYPE_HIT_CEILING:
      jumper.v.y = 0.0f;
      jumper.frame.y = jumper.map_state.ceiling;
      break;

    case MAP_OBJECT_STATE_TYPE_REACHING_TOP:
      jumper.v.y = 0.0f;
      break;

    case MAP_OBJECT_STATE_TYPE_JUMP:
      jumper.v.y *= 1.0f / GRAVITY_ACC;
      jumper.frame.y = jumper.frame.y + jumper.v.y;
      break;

    case MAP_OBJECT_STATE_TYPE_ON_FLOOR:
      jumper.v.y = 0.0f;
      jumper.frame.y = jumper.map_state.floor;
      break;

    case MAP_OBJECT_STATE_TYPE_FALLING:
      if (abs(jumper.v.y) < VELOCITY_ZERO_THRESHOLD) {
        jumper.v.y = VELOCITY_ZERO_THRESHOLD;
      }
      jumper.v.y *= GRAVITY_ACC;

      jumper.frame.y =
          std::min(jumper.frame.y + jumper.v.y, (float)jumper.map_state.floor);
      break;
    }
  }

  { // Death checks.
    if (jumper.frame.y > GetScreenHeight()) {
      is_over = true;
    }
  }
}

void StageGame::draw() {
  /**
   * Window scroll calc:
   * Map:    |------------------------------------------------|
   * Window:           |--------------|
   * Offset:  ---------^
   */
  int scroll_offset = 0;
  if (jumper.frame.x >= WINDOW_SCROLL_PADDING) {
    scroll_offset = std::min(map.width - GetScreenWidth(),
                             (int)jumper.frame.x - WINDOW_SCROLL_PADDING);
  }

  jumper.draw(scroll_offset);
  map.draw(scroll_offset);
}

void StageGame::init() {
  is_over = false;
  jumper.init();
}

std::optional<StageT> StageGame::next_stage() {
  if (is_over) {
    return std::optional<StageT>{STAGE_MENU};
  } else {
    return std::nullopt;
  }
}
