#include "shared_map_schema.h"

bool is_tile_type_solid(TileType t) {
  switch (t) {
    case TILE_GROUND:
    case TILE_DOOR:
    case TILE_NULL:
      return true;
    default:
      return false;
  }
}

TileType char_to_tile_type(char ch) {
  switch (ch) {
    case '.':
      return TILE_AIR;
    case 'g':
      return TILE_GROUND;
    case 's':
      return TILE_START;
    case 'e':
      return TILE_END;
    case 'm':
      return TILE_ENEMY_RANDOM;
    case 'c':
      return TILE_ENEMY_CHASER;
    case '*':
      return TILE_COIN;
    case 'r':
      return TILE_REGEX;
    case 'd':
      return TILE_DOOR;
    default:
      return TILE_NULL;
  }
}
