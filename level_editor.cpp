#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

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
  TILE_TYPE_AIR = 1,
  TILE_TYPE_GROUND = 2,
  TILE_TYPE_START = 3,
  TILE_TYPE_END = 4,
  TILE_TYPE_COIN = 5,
};

static const TileType tile_types[] = {
    TILE_TYPE_NOTHING, TILE_TYPE_AIR, TILE_TYPE_GROUND,
    TILE_TYPE_START,   TILE_TYPE_END, TILE_TYPE_COIN,
};

static const char* tile_type_names[] = {
    "Nothing", "Air", "Ground", "Start", "End", "Coin",
};

struct Tile {
  TileType type = TILE_TYPE_NOTHING;
};

struct Input;

struct InputManager {
  vector<pair<string, string>> events;

  void on_change(Input* input);
  vector<pair<string, string>> get_events() {
    return vector<pair<string, string>>{events};
  }
};

struct Input {
  string value;
  Rectangle frame;
  bool is_active;
  string label;
  InputManager* manager;

  Input(InputManager* manager, string label)
      : frame({0.0f, 0.0f, 128, 18}), label(label), manager(manager) {}

  void set_pos(Vector2 pos) {
    frame.x = pos.x;
    frame.y = pos.y;
  }

  void update() {
    if (!is_active && IsMouseButtonPressed(0) &&
        CheckCollisionPointRec(GetMousePosition(), frame))
      is_active = true;

    if (is_active && IsKeyPressed(KEY_TAB)) is_active = false;

    if (is_active) {
      int ch;
      while ((ch = GetKeyPressed())) {
        if (ch >= 32 && ch <= 126) {
          value.push_back((char)ch);
        } else if (ch == KEY_BACKSPACE) {
          value.pop_back();
        } else if (ch == KEY_ENTER) {
          manager->on_change(this);
        }
      }
    }
  }

  void draw() {
    DrawRectangleRec(frame, WHITE);
    if (is_active) DrawRectangleLinesEx(frame, 2, BLUE);
    DrawText(value.c_str(), frame.x + 4, frame.y + 4, 10, BLACK);
    DrawText(label.c_str(), frame.x + 4, frame.y + 22, 8, WHITE);
  }
};

void InputManager::on_change(Input* input) {
  events.push_back({input->label, input->value});
}

struct App {
  Tile map[MAP_MAX_HEIGHT][MAP_MAX_WIDTH];

  int offsx = -BLOCK_SIZE;
  int offsy = -BLOCK_SIZE;

  int prev_offsx;
  int prev_offsy;
  Vector2 start_mouse_pos;

  int selected_tile_idx = TILE_TYPE_GROUND;

  bool is_bulk_creation;

  InputManager input_manager;
  Input input_window_width;

  App() : input_window_width(&input_manager, "Width") {
    InitWindow(WIN_W, WIN_H, "Level Editor");
    SetTargetFPS(FPS);

    input_window_width.set_pos(Vector2{(float)(GetScreenWidth() - 132),
                                       (float)(GetScreenHeight() - 90)});
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

    {  // Input fields.
      input_window_width.update();
    }
  }

  void draw() {
    pair<int, int> mouse_tile_coord = tile_coord();

    // Map.
    for (int y = 0; y < MAP_MAX_HEIGHT; y++) {
      for (int x = 0; x < MAP_MAX_WIDTH; x++) {
        Vector2 tile_pos{(float)(x * BLOCK_SIZE - offsx),
                         (float)(y * BLOCK_SIZE - offsy)};
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
      draw_tile(tile_types[selected_tile_idx],
                Vector2{(float)(mouse_tile_coord.first * BLOCK_SIZE - offsx),
                        (float)(mouse_tile_coord.second * BLOCK_SIZE - offsy)});
      DrawRectangleLines(mouse_tile_coord.first * BLOCK_SIZE - offsx,
                         mouse_tile_coord.second * BLOCK_SIZE - offsy,
                         BLOCK_SIZE, BLOCK_SIZE, BLACK);
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

      input_window_width.draw();
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
        color = RAYWHITE;
        break;
      case TILE_TYPE_AIR:
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
