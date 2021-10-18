#pragma once

#include "map.h"
#include "raylib.h"
#include "types.h"

struct Jumper : IMapStateUpdatable {
  Rectangle frame;
  Vector2 v;
  MapObjectState map_state;

  void draw(int scroll_offset);
  void init(Vector2 start_pos);

  Rectangle get_frame() const;
  Vector2 get_v() const;
  void set_map_state(MapObjectState &&mos);

  Jumper();
};
