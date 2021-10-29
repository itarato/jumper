#include <cstdio>
#include <fstream>
#include <iostream>
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
#define MAP_MIN_WIDTH 40
#define MAP_MAX_WIDTH 200

using namespace std;

enum TileType {
  TILE_TYPE_NOTHING = 0,
  TILE_TYPE_AIR = 1,
  TILE_TYPE_GROUND = 2,
  TILE_TYPE_START = 3,
  TILE_TYPE_END = 4,
  TILE_TYPE_COIN = 5,
  TILE_TYPE_REGEX = 6,
  TILE_TYPE_DOOR = 7,
};

TileType char_to_tile_type(char ch) {
  switch (ch) {
    case '.':
      return TILE_TYPE_AIR;
    case 'g':
      return TILE_TYPE_GROUND;
    case 's':
      return TILE_TYPE_START;
    case 'e':
      return TILE_TYPE_END;
    case '*':
      return TILE_TYPE_COIN;
    case 'r':
      return TILE_TYPE_REGEX;
    case 'd':
      return TILE_TYPE_DOOR;
    default:
      return TILE_TYPE_NOTHING;
  }
}

char tile_type_to_char(TileType type) {
  switch (type) {
    case TILE_TYPE_AIR:
      return '.';
    case TILE_TYPE_GROUND:
      return 'g';
    case TILE_TYPE_START:
      return 's';
    case TILE_TYPE_END:
      return 'e';
    case TILE_TYPE_COIN:
      return '*';
    case TILE_TYPE_REGEX:
      return 'r';
    case TILE_TYPE_DOOR:
      return 'd';
    default:
      fprintf(stderr, "Unknown tile on tile->char conversion");
      return ' ';
  }
}

static const TileType tile_types[] = {
    TILE_TYPE_NOTHING, TILE_TYPE_AIR,  TILE_TYPE_GROUND, TILE_TYPE_START,
    TILE_TYPE_END,     TILE_TYPE_COIN, TILE_TYPE_REGEX,  TILE_TYPE_DOOR,
};

static const char* tile_type_names[] = {
    "Nothing", "Air", "Ground", "Start", "End", "Coin", "Regex", "Door",
};

struct Tile {
  TileType type = TILE_TYPE_NOTHING;
  string value;

  void reset() {
    type = TILE_TYPE_NOTHING;
    value = "";
  }
};

struct Input {
  static vector<Input*> inputs;

  string value;
  Rectangle frame;
  bool is_active;
  string label;

  Input(string label)
      : frame({0.0f, 0.0f, 128, 18}), is_active(false), label(label) {
    Input::inputs.push_back(this);
  }

  void set_pos(Vector2 pos) {
    frame.x = pos.x;
    frame.y = pos.y;
  }

  void set_value(string new_value) { value = new_value; }

  void update() {
    if (!is_active && IsMouseButtonPressed(0) &&
        CheckCollisionPointRec(GetMousePosition(), frame)) {
      for (auto input : Input::inputs) {
        input->is_active = false;
      }

      is_active = true;
    }

    if (is_active && IsKeyPressed(KEY_TAB)) is_active = false;

    if (is_active) {
      int ch;
      while ((ch = GetKeyPressed())) {
        if (ch >= 32 && ch <= 126) {
          value.push_back((char)ch);
        } else if (ch == KEY_BACKSPACE) {
          if (!value.empty()) value.pop_back();
        }
      }
    }
  }

  bool has_changed() { return is_active && IsKeyPressed(KEY_ENTER); }

  void draw() {
    DrawRectangleRec(frame, WHITE);
    if (is_active) DrawRectangleLinesEx(frame, 2, BLUE);
    DrawText(value.c_str(), frame.x + 4, frame.y + 4, 10, BLACK);
    DrawText(label.c_str(), frame.x + 4, frame.y + 22, 8, WHITE);
  }
};

vector<Input*> Input::inputs{};

struct Button {
  Vector2 pos;
  string label;
  int font_size = 10;

  Button(string label) : label(label) {}

  bool is_clicked() {
    return (IsMouseButtonPressed(0) &&
            CheckCollisionPointRec(GetMousePosition(), frame()));
  }

  Rectangle frame() {
    return Rectangle{pos.x, pos.y,
                     (float)(MeasureText(label.c_str(), font_size) + 8),
                     (float)(font_size + 8)};
  }

  void draw() {
    DrawRectangleRec(frame(), BLUE);
    DrawText(label.c_str(), pos.x + 4, pos.y + 4, font_size, WHITE);
  }
};

struct App {
  Tile map[MAP_WINDOW_HEIGHT][MAP_MAX_WIDTH];

  int offsx = -BLOCK_SIZE;
  int offsy = -BLOCK_SIZE;

  int prev_offsx;
  int prev_offsy;
  Vector2 start_mouse_pos;

  int selected_tile_idx = TILE_TYPE_GROUND;

  bool is_bulk_creation;

  Input input_window_width;
  Input input_map_file_name;
  Input input_tile_value;

  int map_width = MAP_WINDOW_WIDTH;

  Button save_button;

  Tile* selected_tile;

  App()
      : input_window_width("Width"),
        input_map_file_name("Map file"),
        input_tile_value("Tile value"),
        save_button("Save"),
        selected_tile(nullptr) {
    InitWindow(WIN_W, WIN_H, "Level Editor");
    SetTargetFPS(FPS);

    input_window_width.set_pos(Vector2{(float)(GetScreenWidth() - 232),
                                       (float)(GetScreenHeight() - 90)});
    input_window_width.set_value(to_string(MAP_WINDOW_WIDTH));

    input_map_file_name.set_pos(Vector2{(float)(GetScreenWidth() - 232),
                                        (float)(GetScreenHeight() - 50)});
    input_map_file_name.set_value("maps/untitled.jm");

    input_tile_value.set_pos(Vector2{(float)(GetScreenWidth() - 432),
                                     (float)(GetScreenHeight() - 90)});

    save_button.pos.x = GetScreenWidth() - 42;
    save_button.pos.y = GetScreenHeight() - 58;
  }

  void load_map_file(const char* file_name) {
    ifstream map_file{file_name};
    if (!map_file.is_open()) {
      fprintf(stderr, "Cannot open map file");
      exit(EXIT_FAILURE);
    }

    for (int y = 0; y < MAP_WINDOW_HEIGHT; y++) {
      for (int x = 0; x < MAP_MAX_WIDTH; x++) {
        map[y][x].reset();
      }
    }

    string line;
    int width;

    for (int i = 0; i < MAP_WINDOW_HEIGHT; i++) {
      getline(map_file, line);

      if (i == 0) {
        width = line.size();
        map_width = width;
        input_window_width.set_value(to_string(width));
      }

      for (int j = 0; j < (int)line.size(); j++) {
        map[i][j].type = char_to_tile_type(line.at(j));
      }
    }
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
    {  // Input fields.
      if (input_window_width.has_changed()) {
        int new_width = stoi(input_window_width.value);
        if (new_width > MAP_MAX_WIDTH) {
          input_window_width.set_value(to_string(MAP_MAX_WIDTH));
          map_width = MAP_MAX_WIDTH;
        } else if (new_width < MAP_MIN_WIDTH) {
          input_window_width.set_value(to_string(MAP_MIN_WIDTH));
          map_width = MAP_MIN_WIDTH;
        } else {
          map_width = new_width;
        }
      }

      if (input_tile_value.has_changed() && selected_tile) {
        selected_tile->value = input_tile_value.value;
      }
    }

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
      input_map_file_name.update();
      input_tile_value.update();
    }

    {  // Button.
      if (save_button.is_clicked()) {
        save_map();
        // cout << "Map saved: " << ;
      }
    }

    {  // Tile selection.
      if (IsMouseButtonPressed(1) && mouse_in_max_frame()) {
        auto selected_tile_coord = tile_coord();
        selected_tile =
            &map[selected_tile_coord.second][selected_tile_coord.first];
        input_tile_value.value = selected_tile->value;
      }
    }
  }

  void draw() {
    pair<int, int> mouse_tile_coord = tile_coord();

    // Map.
    for (int y = 0; y < MAP_WINDOW_HEIGHT; y++) {
      for (int x = 0; x < map_width; x++) {
        Vector2 tile_pos{(float)(x * BLOCK_SIZE - offsx),
                         (float)(y * BLOCK_SIZE - offsy)};
        draw_tile(map[y][x].type, tile_pos);

        if (selected_tile == &map[y][x]) {
          DrawRectangleLines(x * BLOCK_SIZE - offsx, y * BLOCK_SIZE - offsy,
                             BLOCK_SIZE, BLOCK_SIZE, BLACK);
        }

        if (!map[y][x].value.empty()) {
          DrawText(map[y][x].value.c_str(), x * BLOCK_SIZE - offsx + 2,
                   y * BLOCK_SIZE - offsy + 2, 6, BLACK);
        }
      }
    }

    // Window frame.
    DrawRectangleLines(0 - offsx, 0 - offsy, map_width * BLOCK_SIZE,
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
        DrawText(tile_type_names[i], i * 96 + 32, GetScreenHeight() - 30, 20,
                 WHITE);
      }

      input_window_width.draw();
      input_map_file_name.draw();
      input_tile_value.draw();

      save_button.draw();
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
           mouse_tile_coord.first < map_width &&
           mouse_tile_coord.second < MAP_WINDOW_HEIGHT;
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
      case TILE_TYPE_REGEX:
        color = BLUE;
        break;
      case TILE_TYPE_DOOR:
        color = MAGENTA;
        break;
      default:
        break;
    }
    DrawRectangleRec(frame, color);
  }

  int tile_count() const { return sizeof(tile_types) / sizeof(TileType); }

  void save_map() {
    ofstream map_file;
    map_file.open(input_map_file_name.value,
                  ofstream::out | ofstream::in | ofstream::trunc);

    if (!map_file.is_open()) {
      fprintf(stderr, "Cannot write to file");
      return;
    }

    // Tiles.
    for (int y = 0; y < MAP_WINDOW_HEIGHT; y++) {
      for (int x = 0; x < map_width; x++) {
        map_file.put(tile_type_to_char(map[y][x].type));
      }
      map_file.put('\n');
    }

    // Values.
    for (int y = 0; y < MAP_WINDOW_HEIGHT; y++) {
      for (int x = 0; x < map_width; x++) {
        if (map[y][x].value.empty()) continue;

        map_file << x << " " << y << " " << map[y][x].value << endl;
      }
    }
  }
};

int main(int argc, char** argv) {
  App app{};

  if (argc >= 1) {
    app.load_map_file(argv[1]);
  }

  app.run();
}
