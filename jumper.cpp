#include "jumper.h"

#include <string>
#include <vector>

#include "asset_manager.h"
#include "defines.h"
#include "util.h"

#define JUMPER_WIDTH 28
#define JUMPER_HEIGHT 20
#define JUMPER_STEP_SLOWNESS 3
#define JUMPER_STAND_SLOWNESS 6

/**
 * - Jumper pos represents bottom left corner of its bounding rect.
 */

Jumper::Jumper()
    : move_sprite({IMG_LADYBUG_MOVE_0, IMG_LADYBUG_MOVE_1, IMG_LADYBUG_MOVE_2,
                   IMG_LADYBUG_MOVE_3},
                  JUMPER_STEP_SLOWNESS),
      stand_sprite({IMG_LADYBUG_STAND_0, IMG_LADYBUG_STAND_1,
                    IMG_LADYBUG_STAND_2, IMG_LADYBUG_STAND_3},
                   JUMPER_STAND_SLOWNESS) {}

void Jumper::update(Map *map) {
  {  // Horizontal movement.
    if (IsKeyDown(KEY_LEFT)) {
      v.x = std::min(-JUMPER_HMOVE_V, v.x * FRICTION);

      if (IsKeyPressed(KEY_LEFT_CONTROL)) {
        v.x = -5 * JUMPER_HMOVE_V;
      }
    } else if (IsKeyDown(KEY_RIGHT)) {
      v.x = std::max(JUMPER_HMOVE_V, v.x * FRICTION);

      if (IsKeyPressed(KEY_LEFT_CONTROL)) {
        v.x = 5 * JUMPER_HMOVE_V;
      }
    } else {
      v.x *= FRICTION;
      if (fabsf(v.x) < VELOCITY_ZERO_THRESHOLD) {
        v.x = 0.0f;
      }
    }

    if (v.x < 0.0f) {  // Going left.
      int left_wall_x = map->next_left(frame).value_or(0);

      // Door check.
      if (frame.x + v.x <= (float)left_wall_x) {
        LOG_INFO("FOUND DOOR");
        IntVector2D top_left = top_left_block_coord(rec_plus_vector2(frame, v));
        IntVector2D bottom_left = bottom_left_block_coord(rec_plus_vector2(frame, v));

        if (map->tile_of_coord(top_left) == TILE_DOOR) {
          map->open_door(top_left);
        } else if (map->tile_of_coord(bottom_left) == TILE_DOOR) {
          map->open_door(bottom_left);
        }
      }

      frame.x = std::max((int)(frame.x + v.x), left_wall_x);
    } else if (v.x > 0.0f) {  // Going right.
      int right_wall_x =
          map->next_right(frame).value_or(map->block_width * BLOCK_SIZE) -
          frame.width;

      // Door check.
      if (frame.x + v.x >= (float)right_wall_x) {
        LOG_INFO("FOUND DOOR");
        IntVector2D top_right = top_right_block_coord(rec_plus_vector2(frame, v));
        IntVector2D bottom_right = bottom_right_block_coord(rec_plus_vector2(frame, v));

        if (map->tile_of_coord(top_right) == TILE_DOOR) {
          map->open_door(top_right);
        } else if (map->tile_of_coord(bottom_right) == TILE_DOOR) {
          map->open_door(bottom_right);
        }
      }

      frame.x = std::min((int)(frame.x + v.x), right_wall_x);
    }
  }

  {  // Vertical movement.
    map->evaluate_map_object_state(this);

    switch (map_state.type) {
      case MAP_OBJECT_VERTICAL_STATE_HIT_CEILING:
        v.y = 0.0f;
        frame.y = map_state.ceiling;
        break;

      case MAP_OBJECT_VERTICAL_STATE_REACHING_TOP:
        v.y = VELOCITY_ZERO_THRESHOLD;
        break;

      case MAP_OBJECT_VERTICAL_STATE_JUMP:
        v.y *= 1.0f / GRAVITY_ACC;
        frame.y += v.y;
        break;

      case MAP_OBJECT_VERTICAL_STATE_ON_FLOOR:
        v.y = 0.0f;
        frame.y = map_state.floor;
        double_jump.reset();
        break;

      case MAP_OBJECT_VERTICAL_STATE_FALLING:
        if (IsKeyDown(KEY_LEFT_ALT)) {
          v.y = PARACHUTE_V;
        } else {
          v.y *= GRAVITY_ACC;
        }
        frame.y += v.y;
        break;
    }
  }

  {  // Vertical movement.
    if (IsKeyPressed(KEY_SPACE) && double_jump.can_jump(map_state.type)) {
      v.y -= JUMP_FORCE;
    }
  }
}

void Jumper::draw(int scroll_offset) {
  std::string image_name;
  if (v.x == 0.0) {
    image_name = stand_sprite.current_img();
    stand_sprite.progress();
  } else {
    image_name = move_sprite.current_img();
    move_sprite.progress();
  }

  if (v.x < 0.0f) {
    is_facing_right = false;
  } else if (v.x > 0.0f) {
    is_facing_right = true;
  }

  if (is_facing_right) {
    DrawTextureRec(asset_manager.textures[image_name],
                   Rectangle{0.0f, 0.0f, -frame.width, frame.height},
                   Vector2{frame.x - scroll_offset, frame.y}, WHITE);
  } else {
    DrawTexture(asset_manager.textures[image_name], frame.x - scroll_offset,
                frame.y, WHITE);
  }
}

void Jumper::init(Vector2 start_pos) {
  frame.x = start_pos.x;
  frame.y = start_pos.y;
  frame.width = JUMPER_WIDTH;
  frame.height = JUMPER_HEIGHT;

  is_facing_right = true;

  v.x = 0.0f;
  v.y = 0.0f;
}

Rectangle Jumper::get_frame() const { return frame; }
Vector2 Jumper::get_v() const { return v; }
void Jumper::set_map_state(MapObjectState mos) { map_state = mos; }
