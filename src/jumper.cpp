#include "jumper.h"

#include <string>
#include <vector>

#include "asset_manager.h"
#include "defines.h"
#include "shared/util.h"

#define JUMPER_WIDTH 28
#define JUMPER_HEIGHT 20
#define JUMPER_STEP_SLOWNESS 3
#define JUMPER_STAND_SLOWNESS 6

Jumper::Jumper()
    : move_sprite({IMG_LADYBUG_MOVE_0, IMG_LADYBUG_MOVE_1, IMG_LADYBUG_MOVE_2,
                   IMG_LADYBUG_MOVE_3},
                  JUMPER_STEP_SLOWNESS),
      stand_sprite({IMG_LADYBUG_STAND_0, IMG_LADYBUG_STAND_1,
                    IMG_LADYBUG_STAND_2, IMG_LADYBUG_STAND_3},
                   JUMPER_STAND_SLOWNESS) {}

void Jumper::update(Map *map) {
  {// Horizontal movement.
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

    {// Door check.
      Rectangle planned_next_frame{rec_plus_vector2(frame, v)};
      for (auto &corner : corner_block_coords(planned_next_frame)) {
        Tile &tile = map->get_tile(corner);
        if (tile.type == TILE_DOOR && tile.is_enabled) {
          try {
            if (std::regex_search(tile.value, get_regex())) {
              tile.disable();
            }
          } catch (...) {
            _is_dead = true;
          }
        }
      }
    }

    if (v.x < 0.0f) {// Going left.
      int left_wall_x = map->next_left(frame).value_or(0);
      frame.x = std::max((int) (frame.x + v.x), left_wall_x);
    } else if (v.x > 0.0f) {// Going right.
      int right_wall_x =
              map->next_right(frame).value_or(map->block_width * BLOCK_SIZE) -
              frame.width;
      frame.x = std::min((int) (frame.x + v.x), right_wall_x);
    }
  }

  {// Vertical movement.
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
          v.y = std::min(v.y * GRAVITY_ACC, JUMPER_MAX_VERTICAL_SPEED);
        }
        frame.y += v.y;
        break;
    }
  }

  {// Vertical movement.
    if (IsKeyPressed(KEY_SPACE) && double_jump.can_jump(map_state.type)) {
      v.y -= JUMP_FORCE;
    }
  }

  {// Regex collision check.
    auto tile_coords = corner_block_coords(frame);
    for (const auto &tile_coord : tile_coords) {
      Tile &tile = map->get_tile(tile_coord);
      if (tile.type == TILE_REGEX && tile.is_enabled) {
        merge_pattern(regex_raw, tile.value);
        tile.disable();
      }
    }
  }
}

void Jumper::draw(IntVector2D scroll_offset) {
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
                   Vector2{frame.x - scroll_offset.x, frame.y - scroll_offset.y}, WHITE);
  } else {
    DrawTexture(asset_manager.textures[image_name], frame.x - scroll_offset.x,
                frame.y - scroll_offset.y, WHITE);
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

  regex_raw.clear();

  _is_dead = false;
}

Rectangle Jumper::get_frame() const { return frame; }
Vector2 Jumper::get_v() const { return v; }
void Jumper::set_map_state(MapObjectState mos) { map_state = mos; }

std::regex Jumper::get_regex() const {
  std::string raw{regex_raw};

  raw.insert(0, 1, '^');
  raw.push_back('$');

  return std::regex{raw};
}

bool Jumper::is_dead() const {
  return _is_dead;
}
