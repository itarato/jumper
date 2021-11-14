#pragma once

typedef enum {
  TILE_NULL = '\0',
  TILE_AIR = '.',
  TILE_GROUND = 'g',
  TILE_START = 's',
  TILE_END = 'e',
  TILE_ENEMY_RANDOM = 'm',
  TILE_ENEMY_CHASER = 'c',
  TILE_COIN = '*',
  TILE_REGEX = 'r',
  TILE_DOOR = 'd',
  TILE_TRAP = 't',
} TileType;

bool is_tile_type_solid(TileType t);
TileType char_to_tile_type(char ch);
