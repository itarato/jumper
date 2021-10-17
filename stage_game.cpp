#include "stage_game.h"

#include <algorithm>
#include <cmath>

#include "util.h"

#define JUMPER_HMOVE_V 5.0f
#define GRAVITY_ACC 1.1f
#define VELOCITY_ZERO_THRESHOLD 1.0f
#define JUMP_FORCE 16.0f

#define WINDOW_SCROLL_PADDING 256

void StageGame::update() {
  { // Horizontal movement.
    if (IsKeyDown(KEY_LEFT)) {
      jumper.v.x = -JUMPER_HMOVE_V;
    } else if (IsKeyDown(KEY_RIGHT)) {
      jumper.v.x = JUMPER_HMOVE_V;
    } else {
      jumper.v.x = 0.0f;
    }

    // LOG_INFO("Jumper vx:%2f", jumper.v.x);

    if (jumper.v.x < 0.0f) { // Going left.
      float left_wall_x = map.next_left(jumper.frame).value_or(0);
      // LOG_INFO("Jumperx: %.1f Left: %.2f", jumper.frame.x, left_wall_x);
      jumper.frame.x = std::max(jumper.frame.x + jumper.v.x, left_wall_x);
    } else if (jumper.v.x > 0.0f) { // Going right.
      float right_wall_x =
          map.next_right(jumper.frame).value_or(map.width) - jumper.frame.width;
      // LOG_INFO("Right wall: %.2f", right_wall_x);
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

  {                          // Vertical movement.
    if (jumper.v.y < 0.0f) { // Going up.
      float ceiling_y =
          map.next_ceiling(jumper.frame).value_or(-GetScreenHeight());

      // Debug:
      // DrawLine(0, ceiling_y, GetScreenWidth(), ceiling_y, GREEN);

      if (jumper.frame.y + jumper.v.y <= ceiling_y) { // Hit ceiling.
        jumper.v.y = 0.0f;
        jumper.frame.y = ceiling_y;
      } else if (abs(jumper.v.y) < VELOCITY_ZERO_THRESHOLD) {
        jumper.v.y = 0.0f;
      } else { // Can go up.
        jumper.v.y *= 1.0f / GRAVITY_ACC;
        jumper.frame.y = std::max(jumper.frame.y + jumper.v.y, ceiling_y);
      }
    } else { // Going down.
      // Extract next floor for player:
      // This should check [player.y - threshold .. screen_height] for the next
      // floor.
      int floor_y =
          map.next_floor(jumper.frame).value_or(2 * GetScreenHeight()) -
          jumper.frame.height;

      // Check if player is on floor:
      if (abs(jumper.frame.y - floor_y) < PROXIMITY_TRESHOLD) { // On floor.
        jumper.v.y = 0.0f;
        jumper.frame.y = floor_y;
      } else { // In air.
        if (abs(jumper.v.y) < VELOCITY_ZERO_THRESHOLD) {
          jumper.v.y = VELOCITY_ZERO_THRESHOLD;
        }
        jumper.v.y *= GRAVITY_ACC;

        jumper.frame.y = std::min(jumper.frame.y + jumper.v.y, (float)floor_y);
      }
    }
  }

  { // Death checks.
    if (jumper.frame.y > GetScreenHeight()) {
      is_over = true;
    }
  }

  // LOG_INFO("Jumper x:%.2f", jumper.frame.x);
}

void StageGame::draw() {
  // Calculate view window:
  // Jumper is at: jumper.frame.x
  // Map is map.width() wide

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
