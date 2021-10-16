#include "stage_game.h"
#include "coordinator.h"
#include "util.h"
#include <algorithm>
#include <cmath>

#define JUMPER_HMOVE_V 5.0f
#define FLOOR_TRESHOLD 2.0f
#define GRAVITY_ACC 1.07f
#define VELOCITY_ZERO_THRESHOLD 1.0f
#define JUMP_FORCE 10.0f

void StageGame::update() {
  { // Horizontal movement.
    if (IsKeyDown(KEY_LEFT)) {
      Coordinator::move(&jumper, Vector2{-JUMPER_HMOVE_V, 0});
    }
    if (IsKeyDown(KEY_RIGHT)) {
      Coordinator::move(&jumper, Vector2{JUMPER_HMOVE_V, 0});
    }
  }

  {                          // Vertical movement.
    if (jumper.v.y < 0.0f) { // Going up.
      LOG_INFO("up");
      float ceiling_y = -jumper.bounds.y;
      if (abs(jumper.pos.y - ceiling_y) < FLOOR_TRESHOLD) { // Hit ceiling.
        jumper.v.y = 0.0f;
        jumper.pos.y = ceiling_y;
      } else if (abs(jumper.v.y) < VELOCITY_ZERO_THRESHOLD) {
        jumper.v.y = 0.0f;
      } else { // Can go up.
        jumper.v.y *= 1.0f / GRAVITY_ACC;
        jumper.pos.y = std::max(jumper.pos.y + jumper.v.y, ceiling_y);
      }
    } else { // Going down.
      LOG_INFO("down");
      // Extract next floor for player:
      // This should check [player.y - threshold .. screen_height] for the next
      // floor.
      int floor_y = GetScreenHeight();

      // Check if player is on floor:
      if (abs(jumper.pos.y - floor_y) < FLOOR_TRESHOLD) { // On floor.
        jumper.v.y = 0.0f;
        jumper.pos.y = floor_y;
      } else { // In air.
        if (abs(jumper.v.y) < VELOCITY_ZERO_THRESHOLD) {
          jumper.v.y = VELOCITY_ZERO_THRESHOLD;
        }
        jumper.v.y *= GRAVITY_ACC;

        jumper.pos.y = std::min(jumper.pos.y + jumper.v.y, (float)floor_y);
      }
    }

    { // Jump.
      if (IsKeyPressed(KEY_SPACE)) {
        jumper.v.y -= JUMP_FORCE;
      }
    }
  }
}

void StageGame::draw() { jumper.draw(); }

void StageGame::init() { is_over = false; }

std::optional<StageT> StageGame::next_stage() {
  if (is_over) {
    return std::optional<StageT>{STAGE_MENU};
  } else {
    return std::nullopt;
  }
}
