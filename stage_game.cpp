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
  if (state == GAME_STATE_PLAY) {
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
        int right_wall_x = map.next_right(jumper.frame)
                               .value_or(map.block_width * BLOCK_SIZE) -
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
  }

  {  // Completion checks.
    if (jumper.frame.y > GetScreenHeight()) {
      state = GAME_STATE_WAIT_TO_COMPLETE;
      is_victory = false;
    }

    Rectangle end_rec{map.end_pos.x, map.end_pos.y, BLOCK_SIZE, BLOCK_SIZE};
    if (CheckCollisionRecs(end_rec, jumper.frame)) {
      state = GAME_STATE_WAIT_TO_COMPLETE;
      is_victory = true;
    }

    if (state == GAME_STATE_WAIT_TO_COMPLETE) {
      wait_to_complete_timeout++;

      if (wait_to_complete_timeout >= WAIT_TO_COMPLETE_FRAMES) {
        state = GAME_STATE_COMPLETE;
      }
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

  if (state == GAME_STATE_WAIT_TO_COMPLETE) {
    if (is_victory) {
      draw_text_align_center("[v] VICTORY [v]", 128, DARKGRAY);
    } else {
      draw_text_align_center("[x] GAME OVER [x]", 128, DARKGRAY);
    }
  }
}

void StageGame::init() {
  state = GAME_STATE_PLAY;
  jumper.init(map.start_pos);
  wait_to_complete_timeout = 0;
  victory_text.init()->with_aligned_center();
  game_over_text.init()->with_aligned_center();
}

std::optional<StageT> StageGame::next_stage() {
  if (state == GAME_STATE_COMPLETE) {
    return std::optional<StageT>{STAGE_MENU};
  } else {
    return std::nullopt;
  }
}
