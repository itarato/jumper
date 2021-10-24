#include <iostream>
#include <utility>

#include "raylib.h"

#define WIN_H 800
#define WIN_W 1600
#define FPS 120

#define BLOCK_SIZE 32
#define MAP_WINDOW_WIDTH 40
#define MAP_WINDOW_HEIGHT 20
#define MAP_MAX_WIDTH 200
#define MAP_MAX_HEIGHT 20

using namespace std;

enum TileType {
  TILE_TYPE_NOTHING = 0,
  TILE_TYPE_GROUND = 1,
  TILE_TYPE_START = 2,
  TILE_TYPE_END = 3,
  TILE_TYPE_COIN = 4,
};

static const TileType tile_types[] = {
    TILE_TYPE_NOTHING, TILE_TYPE_GROUND, TILE_TYPE_START,
    TILE_TYPE_END,     TILE_TYPE_COIN,
};

static const char* tile_type_names[] = {
    "Nothing", "Ground", "Start", "End", "Coin",
};

struct Tile {
  TileType type = TILE_TYPE_NOTHING;
};

struct App {
  Tile map[MAP_MAX_HEIGHT][MAP_MAX_WIDTH];

  int offsx = 0;
  int offsy = 0;

  int prev_offsx;
  int prev_offsy;
  Vector2 start_mouse_pos;

  int selected_tile_idx = 1;

  bool is_bulk_creation;

  App() {
    InitWindow(WIN_W, WIN_H, "Level Editor");
    SetTargetFPS(FPS);
  }

  void run() {
    while (!WindowShouldClose()) {
      BeginDrawing();
      ClearBackground(RAYWHITE);

      update();
      draw();

      EndDrawing();
    }
  }

  void update() {
    {  // Drag space.
      if (IsKeyPressed(KEY_LEFT_ALT)) {
        prev_offsx = offsx;
        prev_offsy = offsy;
        start_mouse_pos = GetMousePosition();
      }
      if (IsKeyDown(KEY_LEFT_ALT)) {
        Vector2 mouse_pos = GetMousePosition();
        offsx = prev_offsx - (mouse_pos.x - start_mouse_pos.x);
        offsy = prev_offsy - (mouse_pos.y - start_mouse_pos.y);
      }

      if (mouse_in_max_frame()) {
        pair<int, int> mouse_tile_coord = tile_coord();
        Tile* tile = &map[mouse_tile_coord.second][mouse_tile_coord.first];

        if (IsMouseButtonPressed(0)) {
          is_bulk_creation = tile->type != tile_types[selected_tile_idx];
        }

        if (IsMouseButtonDown(0)) {
          if (is_bulk_creation) {
            map[mouse_tile_coord.second][mouse_tile_coord.first].type =
                tile_types[selected_tile_idx];
          } else {
            map[mouse_tile_coord.second][mouse_tile_coord.first].type =
                TILE_TYPE_NOTHING;
          }
        }
      }
    }

    {  // Tile type switch.
      if (IsKeyPressed(KEY_PAGE_DOWN)) {
        selected_tile_idx =
            (selected_tile_idx + tile_count() - 1) % tile_count();
      }
      if (IsKeyPressed(KEY_PAGE_UP)) {
        selected_tile_idx = (selected_tile_idx + 1) % tile_count();
      }
    }
  }

  void draw() {
    pair<int, int> mouse_tile_coord = tile_coord();

    // Map.
    for (int y = 0; y < MAP_MAX_HEIGHT; y++) {
      for (int x = 0; x < MAP_MAX_WIDTH; x++) {
        Vector2 tile_pos{x * BLOCK_SIZE - offsx, y * BLOCK_SIZE - offsy};
        draw_tile(map[y][x].type, tile_pos);
      }
    }

    // Window frame.
    DrawRectangleLines(0 - offsx, 0 - offsy, MAP_MAX_WIDTH * BLOCK_SIZE,
                       MAP_MAX_HEIGHT * BLOCK_SIZE, RED);
    DrawRectangleLines(0 - offsx, 0 - offsy, MAP_WINDOW_WIDTH * BLOCK_SIZE,
                       MAP_WINDOW_HEIGHT * BLOCK_SIZE, MAGENTA);

    if (mouse_in_max_frame()) {
      // Under-mouse tile.
      DrawRectangle(mouse_tile_coord.first * BLOCK_SIZE - offsx,
                    mouse_tile_coord.second * BLOCK_SIZE - offsy, BLOCK_SIZE,
                    BLOCK_SIZE, RED);
    }

    {  // Overlay
      DrawRectangle(0, GetScreenHeight() - 96, GetScreenWidth(), 96,
                    Fade(BLACK, 0.7f));

      for (int i = 0; i < tile_count(); i++) {
        if (selected_tile_idx == i) {
          DrawRectangleLines(26 + i * 96, GetScreenHeight() - 82, 94, 76,
                             WHITE);
        }

        draw_tile(tile_types[i], Vector2{(float)(32 + i * 96),
                                         (float)(GetScreenHeight() - 76)});
        DrawText(tile_type_names[i], i * 96 + 32, GetScreenHeight() - 30, 16,
                 WHITE);
      }
    }
  }

  pair<int, int> tile_coord() const {
    Vector2 mouse_pos{GetMousePosition()};
    int tile_x = (mouse_pos.x + offsx) / BLOCK_SIZE;
    int tile_y = (mouse_pos.y + offsy) / BLOCK_SIZE;
    if (mouse_pos.x + (float)offsx < 0.0f) tile_x--;
    if (mouse_pos.y + (float)offsy < 0.0f) tile_y--;
    return pair<int, int>{tile_x, tile_y};
  }

  bool mouse_in_max_frame() const {
    pair<int, int> mouse_tile_coord = tile_coord();

    return mouse_tile_coord.first >= 0 && mouse_tile_coord.second >= 0 &&
           mouse_tile_coord.first < MAP_MAX_WIDTH &&
           mouse_tile_coord.second < MAP_MAX_HEIGHT;
  }

  void draw_tile(TileType type, Vector2 pos) const {
    Rectangle frame{pos.x, pos.y, BLOCK_SIZE, BLOCK_SIZE};
    Color color;

    switch (type) {
      case TILE_TYPE_NOTHING:
        color = Fade(BLUE, 0.2);
        break;
      case TILE_TYPE_GROUND:
        color = BROWN;
        break;
      case TILE_TYPE_START:
        color = RED;
        break;
      case TILE_TYPE_END:
        color = YELLOW;
        break;
      case TILE_TYPE_COIN:
        color = ORANGE;
        break;
      default:
        break;
    }
    DrawRectangleRec(frame, color);
  }

  int tile_count() const { return sizeof(tile_types) / sizeof(TileType); }
};

int main() {
  App app{};
  app.run();
}
