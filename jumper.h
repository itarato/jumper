#pragma once

#include <string>

#include "map.h"
#include "raylib.h"
#include "sprite.h"
#include "types.h"

struct DoubleJump {
  int air_jumps;

  DoubleJump() : air_jumps(0) {}

  bool can_jump(MapObjectVerticalState vertical_state) {
    if (vertical_state == MAP_OBJECT_VERTICAL_STATE_ON_FLOOR) {
      air_jumps = 0;
      return true;
    }

    if (air_jumps >= 1) {
      return false;
    }

    air_jumps++;
    return true;
  }

  void reset() { air_jumps = 0; }
};

struct Jumper : IMapStateUpdatable {
  Rectangle frame;
  Vector2 v;
  MapObjectState map_state;
  bool is_facing_right;
  Sprite move_sprite;
  Sprite stand_sprite;
  DoubleJump double_jump;
  std::string regex_raw{};

  void draw(int scroll_offset);
  void update(Map *map);
  void init(Vector2 start_pos);

  [[nodiscard]] Rectangle get_frame() const override;
  [[nodiscard]] Vector2 get_v() const override;
  void set_map_state(MapObjectState mos) override;

  Jumper();
};
