#include "stage_game.h"

#include <algorithm>
#include <cmath>

#include "util.h"

#define JUMPER_HMOVE_V 5.0f
#define GRAVITY_ACC 1.07f
#define VELOCITY_ZERO_THRESHOLD 1.0f
#define JUMP_FORCE 10.0f

void StageGame::update() {
  {  // Horizontal movement.
    if (IsKeyDown(KEY_LEFT)) {
      jumper.v.x = -JUMPER_HMOVE_V;
    } else if (IsKeyDown(KEY_RIGHT)) {
      jumper.v.x = JUMPER_HMOVE_V;
    } else {
      jumper.v.x = 0.0f;
    }

    if (jumper.v.x < 0.0f) {
      float left_wall_x = 0.0f;
      jumper.frame.x = std::max(jumper.frame.x + jumper.v.x, left_wall_x);
    } else if (jumper.v.x > 0.0f) {
      float right_wall_x = (float)GetScreenWidth() - jumper.frame.width;
      jumper.frame.x = std::min(jumper.frame.x + jumper.v.x, right_wall_x);
    }
  }

  {  // Vertical movement.
    if (IsKeyPressed(KEY_SPACE)) {
      jumper.v.y -= JUMP_FORCE;
    }
  }

  {                           // Vertical movement.
    if (jumper.v.y < 0.0f) {  // Going up.
      float ceiling_y =
          map.next_ceiling(jumper.frame).value_or(-GetScreenHeight());

      // Debug:
      DrawLine(0, ceiling_y, GetScreenWidth(), ceiling_y, GREEN);

      if (jumper.frame.y + jumper.v.y <= ceiling_y) {  // Hit ceiling.
        jumper.v.y = 0.0f;
        jumper.frame.y = ceiling_y;
      } else if (abs(jumper.v.y) < VELOCITY_ZERO_THRESHOLD) {
        jumper.v.y = 0.0f;
      } else {  // Can go up.
        jumper.v.y *= 1.0f / GRAVITY_ACC;
        jumper.frame.y = std::max(jumper.frame.y + jumper.v.y, ceiling_y);
      }
    } else {  // Going down.
      // Extract next floor for player:
      // This should check [player.y - threshold .. screen_height] for the next
      // floor.
      int floor_y =
          map.next_floor(jumper.frame).value_or(2 * GetScreenHeight()) -
          jumper.frame.height;

      // Check if player is on floor:
      if (abs(jumper.frame.y - floor_y) < PROXIMITY_TRESHOLD) {  // On floor.
        jumper.v.y = 0.0f;
        jumper.frame.y = floor_y;
      } else {  // In air.
        if (abs(jumper.v.y) < VELOCITY_ZERO_THRESHOLD) {
          jumper.v.y = VELOCITY_ZERO_THRESHOLD;
        }
        jumper.v.y *= GRAVITY_ACC;

        jumper.frame.y = std::min(jumper.frame.y + jumper.v.y, (float)floor_y);
      }
    }
  }

  {  // Death checks.
    if (jumper.frame.y > GetScreenHeight()) {
      is_over = true;
    }
  }
}

void StageGame::draw() {
  jumper.draw();
  map.draw();
}

void StageGame::init() { is_over = false; }

std::optional<StageT> StageGame::next_stage() {
  if (is_over) {
    return std::optional<StageT>{STAGE_MENU};
  } else {
    return std::nullopt;
  }
}
