#include "stage_game.h"

#include <algorithm>
#include <cmath>

#include "util.h"

/**
 * ← / → : move
 * SPACE : jump / double jump
 * ALT   : parachute
 * CTRL  : turbo
 */

#define WINDOW_SCROLL_PADDING 256

void StageGame::update() {
  {  // Horizontal movement.
    if (IsKeyDown(KEY_LEFT)) {
      jumper.v.x = std::min(-JUMPER_HMOVE_V, jumper.v.x * FRICTION);

      if (IsKeyPressed(KEY_LEFT_CONTROL)) {
        jumper.v.x = -5 * JUMPER_HMOVE_V;
      }
    } else if (IsKeyDown(KEY_RIGHT)) {
      jumper.v.x = std::max(JUMPER_HMOVE_V, jumper.v.x * FRICTION);

      if (IsKeyPressed(KEY_LEFT_CONTROL)) {
        jumper.v.x = 5 * JUMPER_HMOVE_V;
      }
    } else {
      jumper.v.x *= FRICTION;
      if (abs(jumper.v.x) < VELOCITY_ZERO_THRESHOLD) {
        jumper.v.x = 0.0f;
      }
    }

    if (jumper.v.x < 0.0f) {  // Going left.
      int left_wall_x = map.next_left(jumper.frame).value_or(0);
      // LOG_INFO("Left wall: %d", left_wall_x);
      jumper.frame.x =
          std::max((int)(jumper.frame.x + jumper.v.x), left_wall_x);
    } else if (jumper.v.x > 0.0f) {  // Going right.
      int right_wall_x =
          map.next_right(jumper.frame).value_or(map.block_width * BLOCK_SIZE) -
          jumper.frame.width;

      // LOG_INFO("Right wall: %d", right_wall_x);
      jumper.frame.x =
          std::min((int)(jumper.frame.x + jumper.v.x), right_wall_x);
    }
  }

  {  // Vertical movement.
    map.evaluate_map_object_state(&jumper);

    switch (jumper.map_state.type) {
      case MAP_OBJECT_VERTICAL_STATE_HIT_CEILING:
        jumper.v.y = 0.0f;
        jumper.frame.y = jumper.map_state.ceiling;
        break;

      case MAP_OBJECT_VERTICAL_STATE_REACHING_TOP:
        jumper.v.y = VELOCITY_ZERO_THRESHOLD;
        break;

      case MAP_OBJECT_VERTICAL_STATE_JUMP:
        jumper.v.y *= 1.0f / GRAVITY_ACC;
        jumper.frame.y += jumper.v.y;
        break;

      case MAP_OBJECT_VERTICAL_STATE_ON_FLOOR:
        jumper.v.y = 0.0f;
        jumper.frame.y = jumper.map_state.floor;
        double_jump.reset();
        break;

      case MAP_OBJECT_VERTICAL_STATE_FALLING:
        if (IsKeyDown(KEY_LEFT_ALT)) {
          jumper.v.y = PARACHUTE_V;
        } else {
          jumper.v.y *= GRAVITY_ACC;
        }
        jumper.frame.y += jumper.v.y;
        break;
    }
  }

  {  // Vertical movement.
    if (IsKeyPressed(KEY_SPACE) && double_jump.can_jump()) {
      jumper.v.y -= JUMP_FORCE;
    }
  }

  {  // Completion checks.
    if (jumper.frame.y > GetScreenHeight()) {
      is_over = true;
    }

    Rectangle end_rec{map.end_pos.x, map.end_pos.y, BLOCK_SIZE, BLOCK_SIZE};
    if (CheckCollisionRecs(end_rec, jumper.frame)) {
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
    scroll_offset = std::min((map.block_width * BLOCK_SIZE) - GetScreenWidth(),
                             (int)jumper.frame.x - WINDOW_SCROLL_PADDING);
  }
  // LOG_INFO("Scroll %d", scroll_offset);

  map.draw(scroll_offset);
  jumper.draw(scroll_offset);
}

void StageGame::init() {
  is_over = false;
  jumper.init(map.start_pos);
}

std::optional<StageT> StageGame::next_stage() {
  if (is_over) {
    return std::optional<StageT>{STAGE_MENU};
  } else {
    return std::nullopt;
  }
}
