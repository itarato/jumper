#include "jumper.h"

#include <cmath>
#include <string>
#include <vector>

#include "asset_manager.h"
#include "defines.h"
#include "input.h"
#include "shared/util.h"

#define JUMPER_WIDTH 28
#define JUMPER_HEIGHT 20
#define JUMPER_STEP_SLOWNESS 3
#define JUMPER_STAND_SLOWNESS 6
#define JUMPER_FLY_SLOWNESS 6

Jumper::Jumper()
    : move_sprite(IMG_FORMAT_SPRITE_LADYBUG_MOVE, JUMPER_STEP_SLOWNESS),
      stand_sprite(IMG_FORMAT_SPRITE_LADYBUG_STAND, JUMPER_STAND_SLOWNESS),
      fly_sprite(IMG_FORMAT_SPRITE_LADYBUG_FLY, JUMPER_FLY_SLOWNESS) {}

void Jumper::update(Map* map) {
  if (state == JumperState::Normal) {
    {  // Horizontal movement.
      float h_force{get_horizontal_axis()};
      if (h_force < 0.0f) {
        is_facing_right = false;

        v.x = std::min(JUMPER_HMOVE_V * h_force, v.x * FRICTION);

        if (is_key_pressed(KEY_LEFT_CONTROL)) {
          v.x = 4 * JUMPER_HMOVE_V * h_force;
          JumperSubject::notify_all(JumperEvent::Dash, JumperEventData{&frame});
        }
      } else if (h_force > 0.0f) {
        is_facing_right = true;

        v.x = std::max(JUMPER_HMOVE_V * h_force, v.x * FRICTION);

        if (is_key_pressed(KEY_LEFT_CONTROL)) {
          v.x = 4 * JUMPER_HMOVE_V * h_force;
          JumperSubject::notify_all(JumperEvent::Dash, JumperEventData{&frame});
        }
      } else {
        v.x *= FRICTION;
        if (fabsf(v.x) < VELOCITY_ZERO_THRESHOLD) {
          v.x = 0.0f;
        }
      }

      {  // Door check.
        Rectangle planned_next_frame{rec_plus_vector2(frame, v)};
        for (auto& corner :
             corner_block_coords(shrink(planned_next_frame, 2.0f))) {
          Tile& tile = map->get_tile(corner);
          if (tile.type == TILE_DOOR && tile.is_enabled()) {
            try {
              Rectangle door_frame{
                  (float)(corner.x * BLOCK_SIZE),
                  (float)(corner.y * BLOCK_SIZE),
                  BLOCK_SIZE,
                  BLOCK_SIZE,
              };

              if (std::regex_search(tile.pattern, get_regex())) {
                tile.disable();
                JumperSubject::notify_all(JumperEvent::OpenDoor,
                                          JumperEventData{&frame, door_frame});
              } else {
                JumperSubject::notify_all(JumperEvent::FailedDoor,
                                          JumperEventData{&frame, door_frame});
              }
            } catch (...) {
              // Incorrect regex.
              death_reason = "Death by bad regular expression!";
              kill();
            }
          }
          if (tile.type == TILE_TRAP) {
            death_reason = "Death by trap!";
            kill();
          }
        }
      }

      if (v.x < 0.0f) {  // Going left.
        int left_wall_x = map->next_left(frame).value_or(0);
        frame.x = std::max((int)(frame.x + v.x), left_wall_x);
      } else if (v.x > 0.0f) {  // Going right.
        int right_wall_x =
            map->next_right(frame).value_or(map->block_width * BLOCK_SIZE) -
            frame.width;
        frame.x = std::min((int)(frame.x + v.x), right_wall_x);
      }
    }

    float ceiling{(float)map->next_ceiling(frame).value_or(0)};
    float floor{(float)map->next_floor(frame).value_or(GetScreenHeight() +
                                                       frame.height * 2) -
                frame.height};

    if (v.y < 0.0f) {
      if (frame.y + v.y <= ceiling) {
        vstate = VerticalState::HIT_CEILING;
      } else if (fabsf(v.y) < VELOCITY_ZERO_THRESHOLD) {
        vstate = VerticalState::REACHING_TOP;
      } else {
        vstate = VerticalState::JUMP;
      }
    } else {
      if (frame.y + v.y + PROXIMITY_THRESHOLD >= floor) {
        vstate = VerticalState::ON_FLOOR;
      } else if (fabsf(v.y) < VELOCITY_ZERO_THRESHOLD) {
        vstate = VerticalState::REACHING_TOP;
      } else {
        vstate = VerticalState::FALLING;
      }
    }

    {  // Vertical movement.
      switch (vstate) {
        case VerticalState::HIT_CEILING:
          v.y = 0.0f;
          frame.y = ceiling;
          break;

        case VerticalState::REACHING_TOP:
          v.y = VELOCITY_ZERO_THRESHOLD;
          break;

        case VerticalState::JUMP:
          if (is_key_down(KEY_SPACE)) {
            v.y *= 1.0f / GRAVITY_ACC;
          } else {
            v.y *= 1.0f / GRAVITY_ACC_FASTER;
          }
          frame.y += v.y;
          break;

        case VerticalState::ON_FLOOR:
          v.y = 0.0f;
          frame.y = floor;
          double_jump.reset();
          break;

        case VerticalState::FALLING:
          if (is_key_down(KEY_LEFT_ALT)) {
            v.y = PARACHUTE_V;
          } else {
            v.y = std::min(v.y * GRAVITY_ACC, JUMPER_MAX_VERTICAL_SPEED);
          }
          frame.y += v.y;
          break;
      }
    }

    {  // Vertical movement.
      if (is_key_pressed(KEY_SPACE) && double_jump.can_jump(vstate)) {
        v.y -= JUMP_FORCE;
        JumperSubject::notify_all(JumperEvent::Jump, JumperEventData{&frame});
      }
    }

    {  // Regex collision check.
      auto tile_coords = corner_block_coords(frame);
      for (const auto& tile_coord : tile_coords) {
        Tile& tile = map->get_tile(tile_coord);
        if (tile.type == TILE_REGEX && tile.is_enabled()) {
          merge_pattern(regex_raw, tile.pattern);
          tile.disable();
          JumperSubject::notify_all(JumperEvent::CaptureRegex,
                                    JumperEventData{&frame});
        }
      }
    }

    {  // Pooper checks.
      if (is_key_down(KEY_X)) {
        if (v.x == 0.0f && v.y == 0.0f) {
          pooper.powerup();
          is_pooping.set();

          if (pooper.is_ready()) {
            JumperSubject::notify_all(JumperEvent::Poop,
                                      JumperEventData{&frame});
            pooper.reset();
          }
        } else {
          pooper.reset();
        }
      } else {
        pooper.reset();
      }
    }

    shield_countdown.tick();
  } else if (state == JumperState::Dying) {
    dying_fade -= 0.01f;
    dying_rot += 8.0f;
    dying_scale *= 1.02f;

    if (dying_fade <= 0.0f) {
      state = JumperState::Dead;
    }
  }

  fly_sprite.progress();
  stand_sprite.progress();
  move_sprite.progress();
}

void Jumper::draw(IntVector2D scroll_offset) {
  Texture2D* image_name{nullptr};
  if (vstate == VerticalState::FALLING && is_key_down(KEY_LEFT_ALT)) {
    image_name = fly_sprite.current_img();
  } else if (v.x == 0.0) {
    image_name = stand_sprite.current_img();
  } else {
    image_name = move_sprite.current_img();
  }

  Rectangle draw_frame{0.0f, 0.0f, is_facing_right ? -frame.width : frame.width,
                       frame.height};

  if (state == JumperState::Normal) {
    Color color{WHITE};
    float fade{1.0f};

    if (is_pooping.get()) {
      color.r = (uint8_t)shade_phaser.value();
    }

    if (!shield_countdown.is_completed()) {
      fade = shield_countdown.get() < 90 ? shield_phaser.value() : 0.2f;
    }

    DrawTextureRec(
        *image_name, draw_frame,
        Vector2{frame.x - scroll_offset.x, frame.y - scroll_offset.y},
        Fade(color, fade));
  } else if (state == JumperState::Dying) {
    DrawTextureEx(*image_name,
                  Vector2{frame.x - scroll_offset.x, frame.y - scroll_offset.y},
                  dying_rot, dying_scale, Fade(WHITE, dying_fade));
  }
}

void Jumper::init(Vector2 start_pos) {
  frame.x = start_pos.x;
  frame.y = start_pos.y;
  frame.width = JUMPER_WIDTH;
  frame.height = JUMPER_HEIGHT;

  is_facing_right = true;
  is_pooping.reset();

  v.x = 0.0f;
  v.y = 0.0f;

  regex_raw.clear();
  shield_countdown.cancel();

  state = JumperState::Normal;

  dying_rot = 0.0f;
  dying_fade = 1.0f;
  dying_scale = 1.0f;

  death_reason = "";
}

std::regex Jumper::get_regex() const {
  std::string raw{regex_raw};

  raw.insert(0, 1, '^');
  raw.push_back('$');

  return std::regex{raw};
}

bool Jumper::is_dead() const { return state == JumperState::Dead; }

bool Jumper::is_alive() const { return state == JumperState::Normal; }

void Jumper::kill() {
  state = JumperState::Dying;
  JumperSubject::notify_all(JumperEvent::Die, JumperEventData{&frame});
}

void Jumper::activate_shield() { shield_countdown.reset(); }

bool Jumper::is_shielded() { return !shield_countdown.is_completed(); }
