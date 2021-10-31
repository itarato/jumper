#include "map.h"

#include <fstream>
#include <utility>

#include "asset_manager.h"
#include "defines.h"
#include "raylib.h"
#include "shared/util.h"

static Tile null_tile{TILE_ERROR};

void Map::build(std::string map_file_path) { load_map(map_file_path); }

void Map::draw(int scroll_offset) {
  for (int v = 0; v < (int) map.size(); v++) {
    for (int h = 0; h < (int) map[v].size(); h++) {
      int block_lhs_x = h * BLOCK_SIZE - scroll_offset;
      int block_rhs_x = block_lhs_x + BLOCK_SIZE;
      if (block_rhs_x <= 0 ||
          block_lhs_x >= (block_width * BLOCK_SIZE)) {// Out of window.
        continue;
      }

      std::string image;
      switch (map[v][h].type) {
        case TILE_GROUND:
          DrawTexture(asset_manager.textures[IMG_GROUND],
                      h * BLOCK_SIZE - scroll_offset, v * BLOCK_SIZE, Fade(WHITE, map[v][h].fade));
          break;

        case TILE_END:
          DrawRectangleLines(h * BLOCK_SIZE - scroll_offset, v * BLOCK_SIZE,
                             BLOCK_SIZE, BLOCK_SIZE, BLACK);
          break;

        case TILE_DOOR:
          if (map[v][h].is_enabled) {
            image = IMG_DOOR_CLOSE;
          } else {
            image = IMG_DOOR_OPEN;
          }
          DrawTexture(asset_manager.textures[image],
                      h * BLOCK_SIZE - scroll_offset, v * BLOCK_SIZE, BROWN);
          break;

        case TILE_REGEX:
          DrawRectangle(h * BLOCK_SIZE - scroll_offset, v * BLOCK_SIZE,
                        BLOCK_SIZE, BLOCK_SIZE, YELLOW);
          break;

        default:
          break;
      }

      if (!map[v][h].value.empty() && map[v][h].is_enabled) {
        DrawRectangleRounded(Rectangle{
                                     (float) h * BLOCK_SIZE - scroll_offset - 4,
                                     (float) v * BLOCK_SIZE - 2,
                                     (float) MeasureText(map[v][h].value.c_str(), 10) + 8,
                                     10.0f + 4,
                             },
                             2.0f, 2, LIME);
        DrawText(map[v][h].value.c_str(), h * BLOCK_SIZE - scroll_offset, v * BLOCK_SIZE, 10, WHITE);
      }
    }
  }
}

std::optional<int> Map::next_floor(Rectangle p) {
  int curr_col_lhs = (int) p.x / BLOCK_SIZE;
  int curr_col_rhs = (int) (p.x + p.width - PROXIMITY_THRESHOLD) / BLOCK_SIZE;

  int curr_row = (int) (p.y + p.height - PROXIMITY_THRESHOLD) / BLOCK_SIZE;

  std::optional<int> floor;

  for (int i = std::max(0, curr_row + 1); i < WINDOW_BLOCK_HEIGHT; i++) {
    if (map[i][curr_col_lhs].is_solid()) {
      floor = std::optional<int>{i * BLOCK_SIZE};
      break;
    }
  }

  if (curr_col_rhs != curr_col_lhs) {
    for (int i = std::max(0, curr_row + 1); i < WINDOW_BLOCK_HEIGHT; i++) {
      if (map[i][curr_col_rhs].is_solid()) {
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
  int curr_col_lhs = (int) p.x / BLOCK_SIZE;
  int curr_col_rhs = (int) (p.x + p.width - PROXIMITY_THRESHOLD) / BLOCK_SIZE;

  int curr_row = (int) p.y / BLOCK_SIZE;

  std::optional<int> ceiling;

  for (int i = std::min(WINDOW_BLOCK_HEIGHT - 1, curr_row - 1); i >= 0; i--) {
    if (map[i][curr_col_lhs].is_solid()) {
      ceiling = std::optional<int>{(i + 1) * BLOCK_SIZE};
      break;
    }
  }

  if (curr_col_lhs != curr_col_rhs) {
    for (int i = std::min(WINDOW_BLOCK_HEIGHT - 1, curr_row - 1); i >= 0; i--) {
      if (map[i][curr_col_rhs].is_solid()) {
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
  int curr_row_top = (int) p.y / BLOCK_SIZE;
  int curr_row_bottom = (int) (p.y + p.height - PROXIMITY_THRESHOLD) / BLOCK_SIZE;

  int curr_col = (int) p.x / BLOCK_SIZE;

  std::optional<int> left;

  if (in_range(curr_row_top, 0, (int) map.size() - 1)) {
    for (int i = std::min(curr_col - 1, block_width - 1); i >= 0; i--) {
      if (map[curr_row_top][i].is_solid()) {
        left = std::optional<int>{(i + 1) * BLOCK_SIZE};
        break;
      }
    }
  }

  if (curr_row_top != curr_row_bottom) {
    if (in_range(curr_row_bottom, 0, (int) map.size() - 1)) {
      for (int i = std::min(curr_col - 1, block_width - 1); i >= 0; i--) {
        if (map[curr_row_bottom][i].is_solid()) {
          int bottom_left = (i + 1) * BLOCK_SIZE;
          left = std::optional<int>{
                  std::max(bottom_left, left.value_or(bottom_left))};
          break;
        }
      }
    }
  }

  return left;
}

std::optional<int> Map::next_right(Rectangle p) {
  int curr_row_top = (int) p.y / BLOCK_SIZE;
  int curr_row_bottom = (int) (p.y + p.height - PROXIMITY_THRESHOLD) / BLOCK_SIZE;

  int curr_col = (int) (p.x + p.width - PROXIMITY_THRESHOLD) / BLOCK_SIZE;

  std::optional<int> right;

  if (in_range(curr_row_top, 0, map.size() - 1)) {
    for (int i = std::max(0, curr_col + 1); i < block_width; i++) {
      if (map[curr_row_top][i].is_solid()) {
        right = std::optional<int>{i * BLOCK_SIZE};
        break;
      }
    }
  }

  if (curr_row_top != curr_row_bottom) {
    if (in_range(curr_row_bottom, 0, map.size() - 1)) {
      for (int i = std::max(0, curr_col + 1); i < block_width; i++) {
        if (map[curr_row_bottom][i].is_solid()) {
          int bottom_right = i * BLOCK_SIZE;
          right = std::optional<int>{
                  std::min(bottom_right, right.value_or(bottom_right))};
          break;
        }
      }
    }
  }

  return right;
}

void Map::evaluate_map_object_state(IMapStateUpdatable *obj) {
  MapObjectState mos{};

  if (obj->get_v().y < 0.0f) {// Going up.
    int ceiling = next_ceiling(obj->get_frame()).value_or(0);
    mos.ceiling = ceiling;

    if (obj->get_frame().y + obj->get_v().y <= ceiling) {// Hit ceiling.
      mos.type = MAP_OBJECT_VERTICAL_STATE_HIT_CEILING;
    } else if (fabs(obj->get_v().y) <
               VELOCITY_ZERO_THRESHOLD) {// Reaching top.
      mos.type = MAP_OBJECT_VERTICAL_STATE_REACHING_TOP;
    } else {// Jump.
      mos.type = MAP_OBJECT_VERTICAL_STATE_JUMP;
    }
  } else {// Going down.
    int floor = next_floor(obj->get_frame()).value_or(2 * GetScreenHeight()) -
                obj->get_frame().height;
    mos.floor = floor;

    if (obj->get_frame().y + obj->get_v().y + PROXIMITY_THRESHOLD >=
        floor) {// On floor.
      mos.type = MAP_OBJECT_VERTICAL_STATE_ON_FLOOR;
    } else if (fabsf(obj->get_v().y) <
               VELOCITY_ZERO_THRESHOLD) {// Reaching top.
      mos.type = MAP_OBJECT_VERTICAL_STATE_REACHING_TOP;
    } else {// Falling down.
      mos.type = MAP_OBJECT_VERTICAL_STATE_FALLING;
    }
  }

  obj->set_map_state(mos);
}

void Map::load_map(const std::string &file_path) {
  std::ifstream file{file_path};
  std::string line;

  map.clear();

  if (!file.is_open()) {
    fprintf(stderr, "Failed opening map file %s", file_path.c_str());
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < WINDOW_BLOCK_HEIGHT; i++) {
    if (!getline(file, line)) {
      fprintf(stderr, "Incomplete map");
      exit(EXIT_FAILURE);
    }

    printf("Read map: %s\n", line.c_str());

    std::vector<Tile> map_line;
    for (auto &ch : line) {
      if (ch == TILE_START) {
        start_pos.x = map_line.size() * BLOCK_SIZE;
        start_pos.y = map.size() * BLOCK_SIZE;
      } else if (ch == TILE_END) {
        end_pos.x = map_line.size() * BLOCK_SIZE;
        end_pos.y = map.size() * BLOCK_SIZE;
      }

      map_line.emplace_back(char_to_tile_type(ch));
    }

    map.push_back(map_line);
  }

  while (getline(file, line)) {
    TileMeta tile_meta{line};
    map[tile_meta.y][tile_meta.x].value = tile_meta.value;
  }

  file.close();

  block_width = (int) map[0].size();
}

bool Map::is_solid_tile(IntVector2D coord) const {
  if (!is_inside_map(coord)) {
    fprintf(stderr, "Map::is_solid_tile out of bounds");
    exit(EXIT_FAILURE);
  }

  return map[coord.y][coord.x].is_solid();
}

std::vector<IntVector2D> Map::coords_of_tile_type(TileType type) {
  std::vector<IntVector2D> out;

  for (std::size_t y = 0; y < map.size(); y++) {
    for (std::size_t x = 0; x < map[0].size(); x++) {
      if (map[y][x].type == type) {
        out.emplace_back((int) x, (int) y);
      }
    }
  }

  return out;
}

Tile &Map::get_tile(IntVector2D coord) {
  if (!is_inside_map(coord)) {
    LOG_INFO("TILE ERROR");
    return null_tile;
  }

  return map[coord.y][coord.x];
}

bool Map::is_inside_map(IntVector2D coord) const {
  return coord.y >= 0 || coord.x >= 0 || coord.y < WINDOW_BLOCK_HEIGHT || coord.x < block_width;
}
