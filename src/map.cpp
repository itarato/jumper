#include "map.h"

#include <fstream>
#include <optional>
#include <utility>

#include "asset_manager.h"
#include "defines.h"
#include "raylib.h"
#include "shared/util.h"

static Tile null_tile{TILE_NULL};

struct TileListIterator {
  IntVector2D scroll_offset;
  int width;
  int height;
  int v{0};
  int h{0};

  TileListIterator(IntVector2D scroll_offset, int width, int height)
      : scroll_offset(scroll_offset), width(width), height(height) {}

  std::optional<std::pair<int, int>> next() {
    for (; v < height; v++) {
      int block_lhs_y = v * BLOCK_SIZE - scroll_offset.y;
      if (block_lhs_y >= GetScreenHeight()) break;

      int block_rhs_y = block_lhs_y + BLOCK_SIZE;
      if (block_rhs_y <= 0) continue;

      for (; h < width; h++) {
        int block_lhs_x = h * BLOCK_SIZE - scroll_offset.x;
        if (block_lhs_x >= GetScreenWidth()) break;

        int block_rhs_x = block_lhs_x + BLOCK_SIZE;
        if (block_rhs_x <= 0) continue;

        return std::optional<std::pair<int, int>>{{v, h++}};
      }

      h = 0;
    }

    return std::nullopt;
  }

  void reset() {
    v = 0;
    h = 0;
  }
};

Texture2D* SingleTextureProvider::texture() {
  return enabled ? texture_enabled : texture_disabled;
}

void SingleTextureProvider::disable() { enabled = false; }
void SingleTextureProvider::enable() { enabled = true; }

Color SingleTextureProvider::color() const { return WHITE; }

Texture2D* DisableSpriteTextureProvider::texture() {
  if (state == SpriteTextureProviderState::Enabled) {
    return texture_enabled;
  } else if (state == SpriteTextureProviderState::Disabled) {
    return texture_disabled;
  } else {
    int idx = step;

    if (frame_counter % step_frame_count == 0) {
      if (state == SpriteTextureProviderState::Disabling) {
        step++;
        if (step >= (int)sprite.size()) {
          state = SpriteTextureProviderState::Disabled;
          step--;
        }
      } else if (state == SpriteTextureProviderState::Enabling) {
        step--;
        if (step < 0) {
          state = SpriteTextureProviderState::Enabled;
          step = 0;
        }
      } else {
        PANIC("Incorrect SpriteTextureProviderState");
      }
    }

    frame_counter++;

    return sprite[idx];
  }
}

void DisableSpriteTextureProvider::disable() {
  SingleTextureProvider::disable();
  state = SpriteTextureProviderState::Disabling;
}

void DisableSpriteTextureProvider::enable() {
  SingleTextureProvider::enable();
  state = SpriteTextureProviderState::Enabling;
}

void Map::build(const std::string& map_file_path) { load_map(map_file_path); }

void Map::update() {
  for (auto& row : map) {
    for (auto& tile : row) {
      tile.update();
    }
  }
}

void Map::draw(IntVector2D scroll_offset) {
  TileListIterator it{scroll_offset, (int)block_width, (int)block_height};
  std::optional<std::pair<int, int>> coords;

  // First run - background tiles.
  for (coords = it.next(); coords.has_value(); coords = it.next()) {
    int v = coords.value().first;
    int h = coords.value().second;

    Tile* tile = &map[v][h];

    Texture2D* texture{tile->texture_provider->texture()};
    if (texture) {
      DrawTextureRec(*texture, tile->draw_frame,
                     Vector2{(float)(h * BLOCK_SIZE - scroll_offset.x),
                             (float)(v * BLOCK_SIZE - scroll_offset.y)},
                     tile->texture_provider->color());
    }
  }

  it.reset();

  // Second run - overlays.
  for (coords = it.next(); coords.has_value(); coords = it.next()) {
    int v = coords.value().first;
    int h = coords.value().second;

    Tile* tile = &map[v][h];

    if (!tile->pattern.empty() && tile->is_enabled()) {
      Vector2 text_frame = MeasureTextEx(
          asset_manager.fonts[FONT_SMALL], tile->formatted_pattern().c_str(),
          asset_manager.fonts[FONT_SMALL].baseSize, 0);

      Color text_bubble_color;
      if (tile->is_regex()) {
        if (tile->is_regex_transformation()) {
          text_bubble_color = DARKBLUE;
        } else {
          text_bubble_color = LIME;
        }
      } else {
        text_bubble_color = DARKPURPLE;
      }

      int text_offset = tile->is_regex_prepend() ? 9 : 0;
      float bubble_width = text_frame.x + 8.0f + text_offset;

      if (tile->is_regex_append()) bubble_width += 9;

      DrawRectangleRounded(
          Rectangle{
              (float)(h * BLOCK_SIZE - scroll_offset.x - 4),
              (float)(v * BLOCK_SIZE - scroll_offset.y - 2),
              bubble_width,
              text_frame.y + 4.0f,
          },
          6.0f, 6, text_bubble_color);

      DrawTextEx(
          asset_manager.fonts[FONT_SMALL], tile->formatted_pattern().c_str(),
          Vector2{(float)(h * BLOCK_SIZE - scroll_offset.x + text_offset),
                  (float)(v * BLOCK_SIZE - scroll_offset.y)},
          asset_manager.fonts[FONT_SMALL].baseSize, 0, WHITE);

      if (tile->is_regex_prepend()) {
        DrawTexture(asset_manager.textures[IMG_REGEX_PREPEND],
                    h * BLOCK_SIZE - scroll_offset.x - 2,
                    v * BLOCK_SIZE - scroll_offset.y + 1, WHITE);
      }
      if (tile->is_regex_append()) {
        DrawTextureRec(
            asset_manager.textures[IMG_REGEX_PREPEND],
            Rectangle{0.0f, 0.0f,
                      (float)-asset_manager.textures[IMG_REGEX_PREPEND].width,
                      (float)asset_manager.textures[IMG_REGEX_PREPEND].height},
            Vector2{(float)(h * BLOCK_SIZE - scroll_offset.x + text_offset +
                            text_frame.x + 2),
                    (float)(v * BLOCK_SIZE - scroll_offset.y + 1)},
            WHITE);
      }
    }

    if (tile->has_decoration()) {
      std::string image_name{
          TextFormat(IMG_FORMAT_DECORATION, tile->decoration)};
      Texture2D* texture = &asset_manager.textures[image_name];

      DrawTexture(*texture,
                  h * BLOCK_SIZE - scroll_offset.x + (BLOCK_SIZE / 2) -
                      (texture->width / 2),
                  (v + 1) * BLOCK_SIZE - scroll_offset.y - texture->height,
                  WHITE);
    }
  }
}

std::optional<int> Map::next_floor(Rectangle p) {
  int curr_col_lhs = (int)p.x / BLOCK_SIZE;
  int curr_col_rhs = (int)(p.x + p.width - PROXIMITY_THRESHOLD) / BLOCK_SIZE;

  int curr_row = (int)(p.y + p.height - PROXIMITY_THRESHOLD) / BLOCK_SIZE;

  std::optional<int> floor;

  for (size_t i = std::max(0, curr_row + 1); i < block_height; i++) {
    if (map[i][curr_col_lhs].is_solid()) {
      floor = std::optional<int>{i * BLOCK_SIZE};
      break;
    }
  }

  if (curr_col_rhs != curr_col_lhs) {
    for (size_t i = std::max(0, curr_row + 1); i < block_height; i++) {
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
  int curr_col_lhs = (int)p.x / BLOCK_SIZE;
  int curr_col_rhs = (int)(p.x + p.width - PROXIMITY_THRESHOLD) / BLOCK_SIZE;

  int curr_row = (int)p.y / BLOCK_SIZE;

  std::optional<int> ceiling;

  for (int i = std::min((int)block_height - 1, curr_row - 1); i >= 0; i--) {
    if (map[i][curr_col_lhs].is_solid()) {
      ceiling = std::optional<int>{(i + 1) * BLOCK_SIZE};
      break;
    }
  }

  if (curr_col_lhs != curr_col_rhs) {
    for (int i = std::min((int)block_height - 1, curr_row - 1); i >= 0; i--) {
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
  int curr_row_top = (int)p.y / BLOCK_SIZE;
  int curr_row_bottom =
      (int)(p.y + p.height - PROXIMITY_THRESHOLD) / BLOCK_SIZE;

  int curr_col = (int)p.x / BLOCK_SIZE;

  std::optional<int> left;

  if (in_range(curr_row_top, 0, (int)map.size() - 1)) {
    for (int i = std::min(curr_col - 1, (int)block_width - 1); i >= 0; i--) {
      if (map[curr_row_top][i].is_solid()) {
        left = std::optional<int>{(i + 1) * BLOCK_SIZE};
        break;
      }
    }
  }

  if (curr_row_top != curr_row_bottom) {
    if (in_range(curr_row_bottom, 0, (int)map.size() - 1)) {
      for (int i = std::min(curr_col - 1, (int)block_width - 1); i >= 0; i--) {
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
  int curr_row_top = (int)p.y / BLOCK_SIZE;
  int curr_row_bottom =
      (int)(p.y + p.height - PROXIMITY_THRESHOLD) / BLOCK_SIZE;

  int curr_col = (int)(p.x + p.width - PROXIMITY_THRESHOLD) / BLOCK_SIZE;

  std::optional<int> right;

  if (in_range(curr_row_top, 0, map.size() - 1)) {
    for (size_t i = std::max(0, curr_col + 1); i < block_width; i++) {
      if (map[curr_row_top][i].is_solid()) {
        right = std::optional<int>{i * BLOCK_SIZE};
        break;
      }
    }
  }

  if (curr_row_top != curr_row_bottom) {
    if (in_range(curr_row_bottom, 0, map.size() - 1)) {
      for (size_t i = std::max(0, curr_col + 1); i < block_width; i++) {
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

void Map::load_map(const std::string& file_path) {
  std::ifstream file{file_path};
  std::string line;

  map.clear();

  if (!file.is_open()) {
    PANIC("Failed opening map file %s", file_path.c_str());
  }

  if (!getline(file, line)) {
    PANIC("Cannot find height");
  }
  block_height = std::stoi(line.c_str());

  for (size_t i = 0; i < block_height; i++) {
    if (!getline(file, line)) {
      PANIC("Incomplete map");
    }

    printf("Read map: %s\n", line.c_str());

    std::vector<Tile> map_line;
    for (auto& ch : line) {
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

    if (tile_meta.has_pattern()) {
      map[tile_meta.y][tile_meta.x].pattern = tile_meta.pattern;
    }

    if (tile_meta.has_decoration()) {
      map[tile_meta.y][tile_meta.x].decoration = tile_meta.decoration;
    }

    if (tile_meta.has_door_timeout()) {
      map[tile_meta.y][tile_meta.x].behaviour =
          std::make_shared<TimedDoorBehaviour>(tile_meta.door_timeout);
    }
  }

  file.close();

  block_width = (int)map[0].size();

  // Post process.
  for (size_t y = 0; y < block_height; y++) {
    for (size_t x = 0; x < block_width; x++) {
      Tile* tile = &map[y][x];

      if (tile->type == TILE_TRAP) {
        if (y > 0 && map[y - 1][x].is_solid()) {
          tile->vertical_flip();
        }
      }
    }
  }
}

bool Map::is_solid_tile(IntVector2D coord) const {
  if (!is_inside_map(coord)) {
    PANIC("Map::is_solid_tile out of bounds");
  }

  return map[coord.y][coord.x].is_solid();
}

std::vector<IntVector2D> Map::coords_of_tile_type(TileType type) {
  std::vector<IntVector2D> out;

  for (std::size_t y = 0; y < map.size(); y++) {
    for (std::size_t x = 0; x < map[0].size(); x++) {
      if (map[y][x].type == type) {
        out.emplace_back((int)x, (int)y);
      }
    }
  }

  return out;
}

Tile& Map::get_tile(IntVector2D coord) {
  if (!is_inside_map(coord)) {
    return null_tile;
  }

  return map[coord.y][coord.x];
}

bool Map::is_inside_map(IntVector2D coord) const {
  return coord.y >= 0 && coord.x >= 0 && coord.y < (int)block_height &&
         coord.x < (int)block_width;
}

size_t Map::pixel_height() const { return block_height * BLOCK_SIZE; }

size_t Map::pixel_width() const { return block_width * BLOCK_SIZE; }
