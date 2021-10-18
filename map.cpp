#include "map.h"

#include <fstream>
#include <utility>

#include "raylib.h"
#include "util.h"

/**
 * Map: 32 bit uint representing map - 20 bit used for blocks.
 *
 * 0b00000000000000000000
 *   ^                  ^
 * floor             ceiling
 */

bool is_tile_steppable(TileType t) {
  switch (t) {
    case TILE_GROUND:
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
    default:
      fprintf(stderr, "Invalid char on map: %c\n", ch);
      exit(EXIT_FAILURE);
  }
}

void read_map_file_to_vec(std::string file_path,
                          std::vector<std::vector<TileType>> &v) {
  std::ifstream file{file_path};
  std::string line;

  if (!file.is_open()) {
    fprintf(stderr, "Failed opening map file %s", file_path.c_str());
    exit(EXIT_FAILURE);
  }

  while (getline(file, line)) {
    printf("Read map: %s\n", line.c_str());

    std::vector<TileType> map_line;
    for (auto &ch : line) {
      map_line.push_back(char_to_tile_type(ch));
    }

    v.push_back(map_line);
  }

  file.close();
}

Map::Map(std::string map_file_path) {
  read_map_file_to_vec(map_file_path, map);
  width = map[0].size();
  LOG_INFO("Width %d", width);
}

void Map::draw(int scroll_offset) {
  for (int v = 0; v < (int)map.size(); v++) {
    for (int h = 0; h < (int)map[v].size(); h++) {
      switch (map[v][h]) {
        case TILE_GROUND:
          DrawRectangle(h * BLOCK_SIZE - scroll_offset, v * BLOCK_SIZE,
                        BLOCK_SIZE, BLOCK_SIZE, ORANGE);
          break;
        default:
          break;
      }
    }
  }
}

std::optional<int> Map::next_floor(Rectangle p) {
  int curr_col_lhs = p.x / BLOCK_SIZE;
  int curr_col_rhs = (p.x + p.width - PROXIMITY_TRESHOLD) / BLOCK_SIZE;

  int curr_row = (p.y + p.height - PROXIMITY_TRESHOLD) / BLOCK_SIZE;

  std::optional<int> floor;

  for (int i = std::max(0, curr_row + 1); i < WINDOW_BLOCK_HEIGHT; i++) {
    if (is_tile_steppable(map[i][curr_col_lhs])) {
      floor = std::optional<int>{i * BLOCK_SIZE};
      break;
    }
  }

  if (curr_col_rhs != curr_col_lhs) {
    for (int i = std::max(0, curr_row + 1); i < WINDOW_BLOCK_HEIGHT; i++) {
      if (is_tile_steppable(map[i][curr_col_rhs])) {
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
    if (is_tile_steppable(map[i][curr_col_lhs])) {
      ceiling = std::optional<int>{(i + 1) * BLOCK_SIZE};
      break;
    }
  }

  if (curr_col_lhs != curr_col_rhs) {
    for (int i = std::min(WINDOW_BLOCK_HEIGHT - 1, curr_row - 1); i >= 0; i--) {
      if (is_tile_steppable(map[i][curr_col_rhs])) {
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

  for (int i = std::min(curr_col - 1, width - 1); i >= 0; i--) {
    if (is_tile_steppable(map[curr_row_top][i])) {
      left = std::optional<int>{(i + 1) * BLOCK_SIZE};
      break;
    }
  }

  if (curr_row_top != curr_row_bottom) {
    for (int i = std::min(curr_col - 1, width - 1); i >= 0; i--) {
      if (is_tile_steppable(map[curr_row_bottom][i])) {
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
  LOG_INFO("JUMPER %.2f %.2f %.2f %.2f", p.x, p.y, p.width, p.height);
  int curr_row_top = p.y / BLOCK_SIZE;
  int curr_row_bottom = (p.y + p.height - PROXIMITY_TRESHOLD) / BLOCK_SIZE;

  int curr_col = (p.x + p.width - PROXIMITY_TRESHOLD) / BLOCK_SIZE;

  std::optional<int> right;

  for (int i = std::max(0, curr_col + 1); i < width; i++) {
    if (is_tile_steppable(map[curr_row_top][i])) {
      LOG_INFO("MAPRIGHT 1 %d", i * BLOCK_SIZE);
      right = std::optional<int>{i * BLOCK_SIZE};
      break;
    }
  }

  if (curr_row_top != curr_row_bottom) {
    for (int i = std::max(0, curr_col + 1); i < width; i++) {
      if (is_tile_steppable(map[curr_row_bottom][i])) {
        int bottom_right = i * BLOCK_SIZE;
        LOG_INFO("MAPRIGHT 2 %d", i * BLOCK_SIZE);
        right = std::optional<int>{
            std::min(bottom_right, right.value_or(bottom_right))};
        break;
      }
    }
  }

  return right;
}

void Map::evaluate_map_object_state(IMapStateUpdatable *obj) {
  MapObjectState mos;

  if (obj->get_v().y < 0.0f) {  // Going up.
    int ceiling = next_ceiling(obj->get_frame()).value_or(-GetScreenHeight());
    mos.ceiling = ceiling;

    if (obj->get_frame().y + obj->get_v().y <= ceiling) {  // Hit ceiling.
      mos.type = MAP_OBJECT_VERTICAL_STATE_HIT_CEILING;
    } else if (abs(obj->get_v().y) <
               VELOCITY_ZERO_THRESHOLD) {  // Reaching top.
      mos.type = MAP_OBJECT_VERTICAL_STATE_REACHING_TOP;
    } else {  // Jump.
      mos.type = MAP_OBJECT_VERTICAL_STATE_JUMP;
    }
  } else {  // Going down.
    int floor = next_floor(obj->get_frame()).value_or(2 * GetScreenHeight()) -
                obj->get_frame().height;
    mos.floor = floor;

    if (obj->get_frame().y + obj->get_v().y + PROXIMITY_TRESHOLD >=
        floor) {  // On floor.
      mos.type = MAP_OBJECT_VERTICAL_STATE_ON_FLOOR;
    } else if (abs(obj->get_v().y) <
               VELOCITY_ZERO_THRESHOLD) {  // Reaching top.
      mos.type = MAP_OBJECT_VERTICAL_STATE_REACHING_TOP;
    } else {  // Falling down.
      mos.type = MAP_OBJECT_VERTICAL_STATE_FALLING;
    }
  }

  obj->set_map_state(std::move(mos));
}
