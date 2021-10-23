#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "raylib.h"
#include "types.h"
#include "util.h"

typedef enum {
  MAP_OBJECT_VERTICAL_STATE_HIT_CEILING = 0,
  MAP_OBJECT_VERTICAL_STATE_REACHING_TOP = 1,
  MAP_OBJECT_VERTICAL_STATE_JUMP = 2,
  MAP_OBJECT_VERTICAL_STATE_ON_FLOOR = 3,
  MAP_OBJECT_VERTICAL_STATE_FALLING = 4,
} MapObjectVerticalState;

struct MapObjectState {
  union {
    int floor;
    int ceiling;
  };
  MapObjectVerticalState type;
};

struct IMapStateUpdatable {
  virtual Rectangle get_frame() const = 0;
  virtual Vector2 get_v() const = 0;
  virtual void set_map_state(MapObjectState &&mos) = 0;
};

typedef enum {
  TILE_AIR = '.',
  TILE_GROUND = 'g',
  TILE_START = 's',
  TILE_END = 'e',
  TILE_ENEMY_RANDOM = 'r',
  TILE_ENEMY_CHASER = 'c',
} TileType;

bool is_tile_steppable(TileType t);
TileType char_to_tile_type(char ch);

struct Map {
  std::vector<std::vector<TileType>> map;
  int block_width;
  // FIXME -> we could generally memoize these
  Vector2 start_pos;
  Vector2 end_pos;

  void draw(int scroll_offset);

  std::optional<int> next_floor(Rectangle p);
  std::optional<int> next_ceiling(Rectangle p);
  std::optional<int> next_left(Rectangle p);
  std::optional<int> next_right(Rectangle p);

  void evaluate_map_object_state(IMapStateUpdatable *obj);
  void load_map(std::string file_path);
  bool is_steppable(int y, int x) const;

  std::vector<IntVector2D> coords_of_tile_type(TileType type);

  Map(std::string map_file_path);
  ~Map() { LOG_INFO("Map dtor"); }
};
