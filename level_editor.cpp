#include <iostream>
#include <utility>

#include "raylib.h"

#define BLOCK_SIZE 32

using namespace std;

enum TileType {
  TILE_TYPE_GROUND = 0,
};

struct App {
  int offsx = 0;
  int offsy = 0;

  int prev_offsx;
  int prev_offsy;
  Vector2 start_mouse_pos;

  TileType selected_tile_type = TILE_TYPE_GROUND;

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

      if (IsMouseButtonPressed(0)) {
        // int tile_piece
      }
    }
  }

  void draw() {
    pair<int, int> mouse_block_coord = tile_coord();

    // Window frame.
    DrawRectangleLines(0 - offsx, 0 - offsy, 1280, 640, MAGENTA);

    // Under-mouse tile.
    DrawRectangle(mouse_block_coord.first * BLOCK_SIZE - offsx,
                  mouse_block_coord.second * BLOCK_SIZE - offsy, BLOCK_SIZE,
                  BLOCK_SIZE, RED);
  }

  pair<int, int> tile_coord() {
    Vector2 mouse_pos{GetMousePosition()};
    int tile_x = (mouse_pos.x + offsx) / BLOCK_SIZE;
    int tile_y = (mouse_pos.y + offsy) / BLOCK_SIZE;
    if (mouse_pos.x + (float)offsx < 0.0f) tile_x--;
    if (mouse_pos.y + (float)offsy < 0.0f) tile_y--;
    return pair<int, int>{tile_x, tile_y};
  }
};

int main() {
  App app{};
  app.run();
}
