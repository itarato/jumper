#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include <cstdlib>

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
  [[nodiscard]] virtual Rectangle get_frame() const = 0;
  [[nodiscard]] virtual Vector2 get_v() const = 0;
  virtual void set_map_state(MapObjectState mos) = 0;
};

typedef enum {
  TILE_ERROR = '\0',
  TILE_AIR = '.',
  TILE_GROUND = 'g',
  TILE_START = 's',
  TILE_END = 'e',
  TILE_ENEMY_RANDOM = 'm',
  TILE_ENEMY_CHASER = 'c',
  TILE_COIN = '*',
  TILE_REGEX = 'r',
  TILE_DOOR = 'd',
} TileType;

bool is_tile_type_solid(TileType t);
TileType char_to_tile_type(char ch);

struct Tile {
  TileType type = TILE_AIR;
  std::string value{};
  bool is_enabled = true;
  float fade;

  explicit Tile (TileType type) : type(type) {
    fade = 0.67f + (float)(rand() % 100) / 300.0f;
  }

  [[nodiscard]] bool is_solid() const { return is_enabled && is_tile_type_solid(type); }
  void disable() { is_enabled = false; }
};

struct Map {
  std::vector<std::vector<Tile>> map;
  int block_width;
  // FIXME -> we could generally memoize these
  Vector2 start_pos;
  Vector2 end_pos;

  void build(std::string map_file_path);
  void draw(int scroll_offset);

  std::optional<int> next_floor(Rectangle p);
  std::optional<int> next_ceiling(Rectangle p);
  std::optional<int> next_left(Rectangle p);
  std::optional<int> next_right(Rectangle p);

  void evaluate_map_object_state(IMapStateUpdatable *obj);
  void load_map(const std::string & file_path);
  [[nodiscard]] bool is_solid_tile(IntVector2D coord) const;
  [[nodiscard]] Tile& get_tile(IntVector2D coord);

  std::vector<IntVector2D> coords_of_tile_type(TileType type);

  [[nodiscard]] bool is_inside_map(IntVector2D coord) const;
};
