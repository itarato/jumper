#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "raylib.h"
#include "shared/shared_defines.h"
#include "shared/shared_map_schema.h"
#include "shared/util.h"

#define WIN_H 800
#define WIN_W 1600
#define FPS 120

#define MIN_WINDOW_BLOCK_WIDTH 40
#define MAX_WINDOW_BLOCK_WIDTH 200

using namespace std;

static const TileType tile_types[] = {
        TILE_ERROR,
        TILE_AIR,
        TILE_GROUND,
        TILE_START,
        TILE_END,
        TILE_ENEMY_RANDOM,
        TILE_ENEMY_CHASER,
        TILE_COIN,
        TILE_REGEX,
        TILE_DOOR,
};

static const char* tile_type_names[] = {
        "Error",
        "Air",
        "Ground",
        "Start",
        "End",
        "EnemyRnd",
        "EnemyCsr",
        "Coin",
        "Regex",
        "Door",
};

char shift_version(char ch) {
  switch (ch) {
    case '1':
      return '!';
    case '4':
      return '$';
    case '6':
      return '^';
    case '8':
      return '*';
    case '9':
      return '(';
    case '0':
      return ')';
    case '[':
      return '{';
    case ']':
      return '}';
    case '=':
      return '+';
    case '-':
      return '_';
    case '/':
      return '?';
    default:
      return ch;
  }
}

struct Tile {
  TileType type = TILE_ERROR;
  string value;

  void reset() {
    type = TILE_ERROR;
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
      : frame({0.0f, 0.0f, 128, 18}),
        is_active(false),
        label(label) {
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
          if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
            value.push_back(shift_version((char) ch));
          } else {
            value.push_back((char) ch);
          }
        } else if (ch == KEY_BACKSPACE) {
          if (!value.empty()) value.pop_back();
        }
      }
    }
  }

  bool has_changed() const { return is_active && IsKeyPressed(KEY_ENTER); }

  void draw() const {
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
                     (float) (MeasureText(label.c_str(), font_size) + 8),
                     (float) (font_size + 8)};
  }

  void draw() {
    DrawRectangleRec(frame(), BLUE);
    DrawText(label.c_str(), pos.x + 4, pos.y + 4, font_size, WHITE);
  }
};

struct App {
  Tile map[WINDOW_BLOCK_HEIGHT][MAX_WINDOW_BLOCK_WIDTH];

  int offsx = -BLOCK_SIZE;
  int offsy = -BLOCK_SIZE;

  int prev_offsx;
  int prev_offsy;
  Vector2 start_mouse_pos;

  int selected_tile_idx = 2;

  bool is_bulk_creation;

  Input input_window_width;
  Input input_map_file_name;
  Input input_tile_value;

  int map_width = WINDOW_BLOCK_WIDTH;

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

    input_window_width.set_pos(Vector2{(float) (GetScreenWidth() - 232),
                                       (float) (GetScreenHeight() - 90)});
    input_window_width.set_value(to_string(WINDOW_BLOCK_WIDTH));

    input_map_file_name.set_pos(Vector2{(float) (GetScreenWidth() - 232),
                                        (float) (GetScreenHeight() - 50)});
    input_map_file_name.set_value("maps/untitled.jm");

    input_tile_value.set_pos(Vector2{(float) (GetScreenWidth() - 432),
                                     (float) (GetScreenHeight() - 90)});

    save_button.pos.x = GetScreenWidth() - 42;
    save_button.pos.y = GetScreenHeight() - 58;
  }

  void load_map_file(const char* file_name) {
    input_map_file_name.value = file_name;

    ifstream map_file{file_name};
    if (!map_file.is_open()) {
      map_width = WINDOW_BLOCK_WIDTH;
      return;
    }

    for (int y = 0; y < WINDOW_BLOCK_HEIGHT; y++) {
      for (int x = 0; x < MAX_WINDOW_BLOCK_WIDTH; x++) {
        map[y][x].reset();
      }
    }

    string line;
    int width;

    for (int i = 0; i < WINDOW_BLOCK_HEIGHT; i++) {
      getline(map_file, line);

      if (i == 0) {
        width = line.size();
        map_width = width;
        input_window_width.set_value(to_string(width));
      }

      for (int j = 0; j < (int) line.size(); j++) {
        map[i][j].type = char_to_tile_type(line.at(j));
      }
    }

    while (getline(map_file, line)) {
      TileMeta tile_meta{line};
      map[tile_meta.y][tile_meta.x].value = tile_meta.value;
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
    {// Input fields.
      if (input_window_width.has_changed()) {
        int new_width = stoi(input_window_width.value);
        if (new_width > MAX_WINDOW_BLOCK_WIDTH) {
          input_window_width.set_value(to_string(MAX_WINDOW_BLOCK_WIDTH));
          map_width = MAX_WINDOW_BLOCK_WIDTH;
        } else if (new_width < MIN_WINDOW_BLOCK_WIDTH) {
          input_window_width.set_value(to_string(MIN_WINDOW_BLOCK_WIDTH));
          map_width = MIN_WINDOW_BLOCK_WIDTH;
        } else {
          map_width = new_width;
        }
      }

      if (input_tile_value.has_changed() && selected_tile) {
        selected_tile->value = input_tile_value.value;
      }
    }

    {// Drag space.
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
                    TILE_ERROR;
          }
        }
      }
    }

    {// Tile type switch.
      if (IsKeyPressed(KEY_PAGE_DOWN)) {
        selected_tile_idx =
                (selected_tile_idx + tile_count() - 1) % tile_count();
      }
      if (IsKeyPressed(KEY_PAGE_UP)) {
        selected_tile_idx = (selected_tile_idx + 1) % tile_count();
      }
    }

    {// Input fields.
      input_window_width.update();
      input_map_file_name.update();
      input_tile_value.update();
    }

    {// Button.
      if (save_button.is_clicked()) {
        save_map();
        // cout << "Map saved: " << ;
      }
    }

    {// Tile selection.
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
    for (int y = 0; y < WINDOW_BLOCK_HEIGHT; y++) {
      for (int x = 0; x < map_width; x++) {
        Vector2 tile_pos{(float) (x * BLOCK_SIZE - offsx),
                         (float) (y * BLOCK_SIZE - offsy)};
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
                       WINDOW_BLOCK_HEIGHT * BLOCK_SIZE, MAGENTA);

    if (mouse_in_max_frame()) {
      // Under-mouse tile.
      draw_tile(tile_types[selected_tile_idx],
                Vector2{(float) (mouse_tile_coord.first * BLOCK_SIZE - offsx),
                        (float) (mouse_tile_coord.second * BLOCK_SIZE - offsy)});
      DrawRectangleLines(mouse_tile_coord.first * BLOCK_SIZE - offsx,
                         mouse_tile_coord.second * BLOCK_SIZE - offsy,
                         BLOCK_SIZE, BLOCK_SIZE, BLACK);
    }

    {// Overlay
      DrawRectangle(0, GetScreenHeight() - 96, GetScreenWidth(), 96,
                    Fade(BLACK, 0.7f));

      for (int i = 0; i < tile_count(); i++) {
        if (selected_tile_idx == i) {
          DrawRectangleLines(26 + i * 96, GetScreenHeight() - 82, 94, 76,
                             WHITE);
        }

        draw_tile(tile_types[i], Vector2{(float) (32 + i * 96),
                                         (float) (GetScreenHeight() - 76)});
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
    if (mouse_pos.x + (float) offsx < 0.0f) tile_x--;
    if (mouse_pos.y + (float) offsy < 0.0f) tile_y--;
    return pair<int, int>{tile_x, tile_y};
  }

  bool mouse_in_max_frame() const {
    pair<int, int> mouse_tile_coord = tile_coord();

    return mouse_tile_coord.first >= 0 && mouse_tile_coord.second >= 0 &&
           mouse_tile_coord.first < map_width &&
           mouse_tile_coord.second < WINDOW_BLOCK_HEIGHT;
  }

  void draw_tile(TileType type, Vector2 pos) const {
    Rectangle frame{pos.x, pos.y, BLOCK_SIZE, BLOCK_SIZE};
    Color color;

    switch (type) {
      case TILE_ERROR:
        color = RAYWHITE;
        break;
      case TILE_AIR:
        color = Fade(BLUE, 0.2);
        break;
      case TILE_GROUND:
        color = BROWN;
        break;
      case TILE_START:
        color = RED;
        break;
      case TILE_END:
        color = YELLOW;
        break;
      case TILE_ENEMY_RANDOM:
        color = LIME;
        break;
      case TILE_ENEMY_CHASER:
        color = SKYBLUE;
        break;
      case TILE_COIN:
        color = ORANGE;
        break;
      case TILE_REGEX:
        color = BLUE;
        break;
      case TILE_DOOR:
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
    for (int y = 0; y < WINDOW_BLOCK_HEIGHT; y++) {
      for (int x = 0; x < map_width; x++) {
        map_file.put((char) map[y][x].type);
      }
      map_file.put('\n');
    }

    // Values.
    for (int y = 0; y < WINDOW_BLOCK_HEIGHT; y++) {
      for (int x = 0; x < map_width; x++) {
        if (map[y][x].value.empty()) continue;

        map_file << x << "," << y << "," << map[y][x].value << endl;
      }
    }
  }
};

int main(int argc, char** argv) {
  App app{};

  if (argc >= 2) {
    app.load_map_file(argv[1]);
  }

  app.run();
}
