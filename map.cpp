#include "map.h"

#include "raylib.h"
#include "util.h"

/**
 * Map: 32 bit uint representing map - 20 bit used for blocks.
 *
 * 0b00000000000000000000
 *   ^                  ^
 * floor             ceiling
 */

Map::Map() {
  for (int i = 0; i < MAP_BLOCK_WIDTH; i++) {
    map[i] = 0b10000000000100000000;
  }
  map[10] = 0b00010000000000000000;
  map[11] = 0b00010000000000000000;
  map[12] = 0b00010000000000000000;
  map[38] = 0b01110000001000010000;
  map[39] = 0b01110000001000010000;
  map[60] = 0b00010000000000000000;
  map[61] = 0b00010000000000000000;
  map[62] = 0b00010000000000000000;

  width = MAP_BLOCK_WIDTH * BLOCK_SIZE;
}

void Map::draw(int scroll_offset) {
  for (int h = 0; h < MAP_BLOCK_WIDTH; h++) {
    uint32_t column = map[h];
    for (int v = 0; v < WINDOW_BLOCK_HEIGHT; v++) {
      if ((column & 1) == 1) {
        DrawRectangle(h * BLOCK_SIZE - scroll_offset, v * BLOCK_SIZE,
                      BLOCK_SIZE, BLOCK_SIZE, ORANGE);
      }
      column >>= 1;
    }
  }
}

std::optional<int> Map::next_floor(Rectangle p) {
  int curr_col_lhs = p.x / BLOCK_SIZE;
  int curr_col_rhs = (p.x + p.width - PROXIMITY_TRESHOLD) / BLOCK_SIZE;

  int curr_row = (p.y + p.height - PROXIMITY_TRESHOLD) / BLOCK_SIZE;

  std::optional<int> floor;

  for (int i = std::max(0, curr_row + 1); i < WINDOW_BLOCK_HEIGHT; i++) {
    if (is_bit_on(map[curr_col_lhs], i)) {
      floor = std::optional<int>{i * BLOCK_SIZE};
      break;
    }
  }

  if (curr_col_rhs != curr_col_lhs) {
    for (int i = std::max(0, curr_row + 1); i < WINDOW_BLOCK_HEIGHT; i++) {
      if (is_bit_on(map[curr_col_rhs], i)) {
        int rhs_floor = i * BLOCK_SIZE;
        floor =
            std::optional<int>{std::min(rhs_floor, floor.value_or(rhs_floor))};
        break;
      }
    }
  }

  return floor;
}

std::optional<int> Map::next_ceiling(Rectangle p) {
  int curr_col_lhs = p.x / BLOCK_SIZE;
  int curr_col_rhs = (p.x + p.width - PROXIMITY_TRESHOLD) / BLOCK_SIZE;

  int curr_row = p.y / BLOCK_SIZE;

  std::optional<int> ceiling;

  for (int i = std::min(WINDOW_BLOCK_HEIGHT - 1, curr_row - 1); i >= 0; i--) {
    if (is_bit_on(map[curr_col_lhs], i)) {
      ceiling = std::optional<int>{(i + 1) * BLOCK_SIZE};
      break;
    }
  }

  if (curr_col_lhs != curr_col_rhs) {
    for (int i = std::min(WINDOW_BLOCK_HEIGHT - 1, curr_row - 1); i >= 0; i--) {
      if (is_bit_on(map[curr_col_rhs], i)) {
        int rhs_ceiling = (i + 1) * BLOCK_SIZE;
        ceiling = std::optional<int>{
            std::max(rhs_ceiling, ceiling.value_or(rhs_ceiling))};
        break;
      }
    }
  }

  return ceiling;
}

std::optional<int> Map::next_left(Rectangle p) {
  int curr_row_top = p.y / BLOCK_SIZE;
  int curr_row_bottom = (p.y + p.height - PROXIMITY_TRESHOLD) / BLOCK_SIZE;

  int curr_col = p.x / BLOCK_SIZE;

  std::optional<int> left;

  // LOG_INFO("TOP: %d BOTTOM: %d", curr_row_top, curr_row_bottom);

  // FIXME - This has a bug - doesn't check if jumper is too on the right (out
  // of bounds).
  for (int i = std::min(curr_col - 1, MAP_BLOCK_WIDTH - 1); i >= 0; i--) {
    if (is_bit_on(map[i], curr_row_top)) {
      left = std::optional<int>{(i + 1) * BLOCK_SIZE};
      break;
    }
  }

  if (curr_row_top != curr_row_bottom) {
    for (int i = std::min(curr_col - 1, MAP_BLOCK_WIDTH - 1); i >= 0; i--) {
      if (is_bit_on(map[i], curr_row_bottom)) {
        int bottom_left = (i + 1) * BLOCK_SIZE;
        left = std::optional<int>{
            std::max(bottom_left, left.value_or(bottom_left))};
        break;
      }
    }
  }

  return left;
}

std::optional<int> Map::next_right(Rectangle p) {
  int curr_row_top = p.y / BLOCK_SIZE;
  int curr_row_bottom = (p.y + p.height - PROXIMITY_TRESHOLD) / BLOCK_SIZE;

  int curr_col = (p.x + p.width - PROXIMITY_TRESHOLD) / BLOCK_SIZE;

  std::optional<int> right;

  for (int i = std::max(0, curr_col + 1); i < MAP_BLOCK_WIDTH; i++) {
    if (is_bit_on(map[i], curr_row_top)) {
      right = std::optional<int>{i * BLOCK_SIZE};
      break;
    }
  }

  if (curr_row_top != curr_row_bottom) {
    for (int i = std::max(0, curr_col + 1); i < MAP_BLOCK_WIDTH; i++) {
      if (is_bit_on(map[i], curr_row_bottom)) {
        int bottom_right = i * BLOCK_SIZE;
        right = std::optional<int>{
            std::min(bottom_right, right.value_or(bottom_right))};
        break;
      }
    }
  }

  return right;
}
