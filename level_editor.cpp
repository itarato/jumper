#include <iostream>
#include <utility>

#include "raylib.h"

#define BLOCK_SIZE 32
#define MAP_WINDOW_WIDTH 40
#define MAP_WINDOW_HEIGHT 20
#define MAP_MAX_WIDTH 200
#define MAP_MAX_HEIGHT 20

using namespace std;

enum TileType {
  TILE_TYPE_NOTHING = 0,
  TILE_TYPE_GROUND = 1,
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

  TileType selected_tile_type = TILE_TYPE_GROUND;

  bool is_bulk_creation;

  App() {
    InitWindow(1600, 800, "Level Editor");
    SetTargetFPS(120);
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
        Tile *tile = &map[mouse_tile_coord.second][mouse_tile_coord.first];

        if (IsMouseButtonPressed(0)) {
          is_bulk_creation = tile->type == TILE_TYPE_NOTHING;
        }

        if (IsMouseButtonDown(0)) {
          if (is_bulk_creation) {
            map[mouse_tile_coord.second][mouse_tile_coord.first].type =
                selected_tile_type;
          } else {
            map[mouse_tile_coord.second][mouse_tile_coord.first].type =
                TILE_TYPE_NOTHING;
          }
        }
      }
    }
  }

  void draw() {
    pair<int, int> mouse_tile_coord = tile_coord();

    // Map.
    for (int y = 0; y < MAP_MAX_HEIGHT; y++) {
      for (int x = 0; x < MAP_MAX_WIDTH; x++) {
        Rectangle frame{x * BLOCK_SIZE - offsx, y * BLOCK_SIZE - offsy,
                        BLOCK_SIZE, BLOCK_SIZE};

        switch (map[y][x].type) {
          case TILE_TYPE_NOTHING:
            break;

          case TILE_TYPE_GROUND:
            DrawRectangleRec(frame, BROWN);
            break;

          default:
            break;
        }
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
  }

  pair<int, int> tile_coord() {
    Vector2 mouse_pos{GetMousePosition()};
    int tile_x = (mouse_pos.x + offsx) / BLOCK_SIZE;
    int tile_y = (mouse_pos.y + offsy) / BLOCK_SIZE;
    if (mouse_pos.x + (float)offsx < 0.0f) tile_x--;
    if (mouse_pos.y + (float)offsy < 0.0f) tile_y--;
    return pair<int, int>{tile_x, tile_y};
  }

  bool mouse_in_max_frame() {
    pair<int, int> mouse_tile_coord = tile_coord();

    return mouse_tile_coord.first >= 0 && mouse_tile_coord.second >= 0 &&
           mouse_tile_coord.first < MAP_MAX_WIDTH &&
           mouse_tile_coord.second < MAP_MAX_HEIGHT;
  }
};

int main() {
  App app{};
  app.run();
}
