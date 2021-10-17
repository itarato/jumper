#pragma once

#include <cstdint>
#include <optional>

#include "raylib.h"
#include "util.h"

#define MAP_BLOCK_WIDTH (WINDOW_BLOCK_WIDTH * 2)

typedef enum {
  MAP_OBJECT_STATE_TYPE_HIT_CEILING = 0,
  MAP_OBJECT_STATE_TYPE_REACHING_TOP = 1,
  MAP_OBJECT_STATE_TYPE_JUMP = 2,
  MAP_OBJECT_STATE_TYPE_ON_FLOOR = 3,
  MAP_OBJECT_STATE_TYPE_FALLING = 4,
} MapObjectStateType;

struct MapObjectState {
  union {
    int floor;
    int ceiling;
  };
  MapObjectStateType type;
};

struct IMapStateUpdatable {
  virtual Rectangle get_frame() const = 0;
  virtual Vector2 get_v() const = 0;
  virtual void set_map_state(MapObjectState &&mos) = 0;
};

struct Map {
  uint32_t map[MAP_BLOCK_WIDTH];
  int width;

  void draw(int scroll_offset);

  std::optional<int> next_floor(Rectangle p);
  std::optional<int> next_ceiling(Rectangle p);
  std::optional<int> next_left(Rectangle p);
  std::optional<int> next_right(Rectangle p);

  void evaluate_map_object_state(IMapStateUpdatable *obj);

  Map();
};
