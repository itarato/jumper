#include "map.h"
#include "raylib.h"

Map::Map() {
  for (int i = 0; i < WINDOW_BLOCK_WIDTH; i++) {
    map[i] = 0b10000000000100000000;
  }
}

void Map::draw() {
  for (int h = 0; h < WINDOW_BLOCK_WIDTH; h++) {
    uint32_t column = map[h];
    for (int v = 0; v < WINDOW_BLOCK_HEIGHT; v++) {
      if (column & 1 == 1) {
        DrawRectangle(h * BLOCK_SIZE, v * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE,
                      ORANGE);
      }
      column >>= 1;
    }
  }
}
