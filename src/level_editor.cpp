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

#define DEFAULT_WINDOW_BLOCK_WIDTH 40
#define DEFAULT_WINDOW_BLOCK_HEIGHT 20
#define MIN_WINDOW_BLOCK_WIDTH 4
#define MIN_WINDOW_BLOCK_HEIGHT 4
#define MAX_WINDOW_BLOCK_WIDTH 200
#define MAX_WINDOW_BLOCK_HEIGHT 200

using namespace std;

static const TileType tile_types[] = {
    TILE_NULL,  TILE_AIR,          TILE_GROUND,       TILE_START,
    TILE_END,   TILE_ENEMY_RANDOM, TILE_ENEMY_CHASER, TILE_COIN,
    TILE_REGEX, TILE_DOOR,         TILE_TRAP,         TILE_SHIELD};

static const char* tile_type_names[] = {
    "NULL",     "Air",  "Ground", "Start", "End",  "EnemyRnd",
    "EnemyCsr", "Coin", "Regex",  "Door",  "Trap", "Shield"};

char char_shift_version(char ch) {
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
  TileType type = TILE_NULL;
  string pattern;
  int decoration;

  void reset() {
    type = TILE_NULL;
    pattern = "";
    decoration = -1;
  }
};

struct Input {
  static vector<Input*> inputs;

  string value;
  Rectangle frame;
  bool is_active;
  string label;

  explicit Input(string label, float width = 128.0f)
      : frame({0.0f, 0.0f, width, 18.0f}),
        is_active(false),
        label(std::move(label)) {
    Input::inputs.push_back(this);
  }

  void set_pos(Vector2 pos) {
    frame.x = pos.x;
    frame.y = pos.y;
  }

  void set_value(string&& new_value) { value = new_value; }

  void update() {
    if (!is_active && IsMouseButtonPressed(0) &&
        CheckCollisionPointRec(GetMousePosition(), frame)) {
      for (auto input : Input::inputs) {
        input->is_active = false;
      }

      is_active = true;
    }

    if (is_active && IsKeyPressed(KEY_TAB))
      is_active = false;

    if (is_active) {
      int ch;
      while ((ch = GetKeyPressed())) {
        if (ch >= 32 && ch <= 126) {
          if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
            value.push_back(char_shift_version((char)ch));
          } else {
            value.push_back((char)ch);
          }
        } else if (ch == KEY_BACKSPACE) {
          if (!value.empty())
            value.pop_back();
        }
      }
    }
  }

  [[nodiscard]] bool has_changed() const {
    return is_active && IsKeyPressed(KEY_ENTER);
  }

  void draw() const {
    DrawRectangleRec(frame, WHITE);
    if (is_active)
      DrawRectangleLinesEx(frame, 2, BLUE);
    DrawText(value.c_str(), frame.x + 4, frame.y + 4, 10, BLACK);
    DrawText(label.c_str(), frame.x + 4, frame.y + 22, 8, WHITE);
  }
};

vector<Input*> Input::inputs{};

struct IButton {
  virtual Rectangle frame() = 0;

  bool is_clicked() {
    return IsMouseButtonPressed(0) &&
           CheckCollisionPointRec(GetMousePosition(), frame());
  }

  virtual void draw() {
    if (CheckCollisionPointRec(GetMousePosition(), frame())) {
      if (IsMouseButtonDown(0)) {
        DrawRectangleLinesEx(frame(), 2.0f, RED);
      } else {
        DrawRectangleLinesEx(frame(), 2.0f, BLUE);
      }
    }
  }
};

struct Button : IButton {
  Vector2 pos{};
  string label{};
  int font_size = 10;

  explicit Button(string label) : label(std::move(label)) {}

  Rectangle frame() override {
    return Rectangle{pos.x, pos.y,
                     (float)(MeasureText(label.c_str(), font_size) + 8),
                     (float)(font_size + 8)};
  }

  void draw() override {
    DrawText(label.c_str(), pos.x + 4, pos.y + 4, font_size, WHITE);

    IButton::draw();
  }
};

struct App {
  Tile map[MAX_WINDOW_BLOCK_HEIGHT][MAX_WINDOW_BLOCK_WIDTH];

  int offsx = -BLOCK_SIZE;
  int offsy = -BLOCK_SIZE;

  int prev_offsx;
  int prev_offsy;
  Vector2 start_mouse_pos;

  int selected_tile_idx = 2;

  bool is_bulk_creation;

  Input input_window_width;
  Input input_window_height;
  Input input_map_file_name;
  Input input_tile_value;
  Input input_decoration_index;

  int map_width = DEFAULT_WINDOW_BLOCK_WIDTH;
  int map_height = DEFAULT_WINDOW_BLOCK_HEIGHT;

  Button save_button;

  Tile* selected_tile;

  App()
      : input_window_width("Width", 64.0f),
        input_window_height("Height", 64.0f),
        input_map_file_name("Map file"),
        input_tile_value("Tile value"),
        input_decoration_index("Decor idx", 48.0f),
        save_button("Save"),
        selected_tile(nullptr) {
    InitWindow(WIN_W, WIN_H, "Level Editor");
    SetTargetFPS(FPS);

    input_window_width.set_pos(Vector2{(float)(GetScreenWidth() - 140),
                                       (float)(GetScreenHeight() - 90)});
    input_window_width.set_value(to_string(DEFAULT_WINDOW_BLOCK_WIDTH));

    input_window_height.set_pos(Vector2{(float)(GetScreenWidth() - 140),
                                        (float)(GetScreenHeight() - 50)});
    input_window_height.set_value(to_string(DEFAULT_WINDOW_BLOCK_HEIGHT));

    input_map_file_name.set_pos(Vector2{(float)(GetScreenWidth() - 280),
                                        (float)(GetScreenHeight() - 50)});
    input_map_file_name.set_value("maps/untitled.jm");

    input_tile_value.set_pos(Vector2{(float)(GetScreenWidth() - 280),
                                     (float)(GetScreenHeight() - 90)});

    input_decoration_index.set_pos(Vector2{(float)(GetScreenWidth() - 60),
                                           (float)(GetScreenHeight() - 90)});

    save_button.pos.x = GetScreenWidth() - 42;
    save_button.pos.y = GetScreenHeight() - 28;
  }

  void load_map_file(const char* file_name) {
    input_map_file_name.value = file_name;

    ifstream map_file{file_name};
    if (!map_file.is_open()) {
      map_width = DEFAULT_WINDOW_BLOCK_WIDTH;
      map_height = DEFAULT_WINDOW_BLOCK_HEIGHT;
      return;
    }

    string line;

    getline(map_file, line);
    map_height = stoi(line);
    input_window_height.set_value(to_string(map_height));

    for (int y = 0; y < MAX_WINDOW_BLOCK_HEIGHT; y++) {
      for (int x = 0; x < MAX_WINDOW_BLOCK_WIDTH; x++) {
        map[y][x].reset();
      }
    }

    for (int i = 0; i < map_height; i++) {
      getline(map_file, line);

      if (i == 0) {
        map_width = (int)line.size();
        input_window_width.set_value(to_string(map_width));
      }

      for (int j = 0; j < (int)line.size(); j++) {
        map[i][j].type = char_to_tile_type(line.at(j));
      }
    }

    while (getline(map_file, line)) {
      TileMeta tile_meta{line};

      if (tile_meta.has_pattern()) {
        map[tile_meta.y][tile_meta.x].pattern = tile_meta.pattern;
      }

      if (tile_meta.has_decoration()) {
        map[tile_meta.y][tile_meta.x].decoration = tile_meta.decoration;
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

      if (input_window_height.has_changed()) {
        int new_height = stoi(input_window_height.value);
        if (new_height > MAX_WINDOW_BLOCK_HEIGHT) {
          input_window_height.set_value(to_string(MAX_WINDOW_BLOCK_HEIGHT));
          map_height = MAX_WINDOW_BLOCK_HEIGHT;
        } else if (new_height < MIN_WINDOW_BLOCK_HEIGHT) {
          input_window_height.set_value(to_string(MIN_WINDOW_BLOCK_HEIGHT));
          map_height = MIN_WINDOW_BLOCK_HEIGHT;
        } else {
          map_height = new_height;
        }
      }

      if (input_tile_value.has_changed() && selected_tile) {
        selected_tile->pattern = input_tile_value.value;
      }

      if (input_decoration_index.has_changed() && selected_tile) {
        selected_tile->decoration = stoi(input_decoration_index.value);
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
                TILE_NULL;
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
      input_window_height.update();
      input_map_file_name.update();
      input_tile_value.update();
      input_decoration_index.update();
    }

    {  // Button.
      if (save_button.is_clicked()) {
        save_map();
        cout << "Map saved.\n";
      }
    }

    {  // Tile selection.
      if (IsMouseButtonPressed(1) && mouse_in_max_frame()) {
        auto selected_tile_coord = tile_coord();
        selected_tile =
            &map[selected_tile_coord.second][selected_tile_coord.first];
        input_tile_value.value = selected_tile->pattern;
        input_decoration_index.value = to_string(selected_tile->decoration);
      }
    }
  }

  void draw() {
    pair<int, int> mouse_tile_coord = tile_coord();

    // Map.
    for (int y = 0; y < map_height; y++) {
      for (int x = 0; x < map_width; x++) {
        Vector2 tile_pos{(float)(x * BLOCK_SIZE - offsx),
                         (float)(y * BLOCK_SIZE - offsy)};
        draw_tile(map[y][x].type, tile_pos);

        if (selected_tile == &map[y][x]) {
          DrawRectangleLines(x * BLOCK_SIZE - offsx, y * BLOCK_SIZE - offsy,
                             BLOCK_SIZE, BLOCK_SIZE, BLACK);
        }

        if (!map[y][x].pattern.empty()) {
          DrawText(map[y][x].pattern.c_str(), x * BLOCK_SIZE - offsx + 2,
                   y * BLOCK_SIZE - offsy + 2, 6, BLACK);
        }

        if (map[y][x].decoration >= 0) {
          DrawText(to_string(map[y][x].decoration).c_str(),
                   x * BLOCK_SIZE - offsx + 2, y * BLOCK_SIZE - offsy + 22, 6,
                   BLACK);
        }
      }
    }

    // Window frame.
    DrawRectangleLines(0 - offsx, 0 - offsy, map_width * BLOCK_SIZE,
                       map_height * BLOCK_SIZE, MAGENTA);

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
          DrawRectangleLines(26 + i * 64, GetScreenHeight() - 82, 64, 76,
                             WHITE);
        }

        draw_tile(tile_types[i], Vector2{(float)(32 + i * 64),
                                         (float)(GetScreenHeight() - 76)});
        DrawText(tile_type_names[i], i * 64 + 32, GetScreenHeight() - 30, 10,
                 WHITE);
      }

      input_window_width.draw();
      input_window_height.draw();
      input_map_file_name.draw();
      input_tile_value.draw();
      input_decoration_index.draw();

      save_button.draw();
    }
  }

  [[nodiscard]] pair<int, int> tile_coord() const {
    Vector2 mouse_pos{GetMousePosition()};
    int tile_x = (mouse_pos.x + offsx) / BLOCK_SIZE;
    int tile_y = (mouse_pos.y + offsy) / BLOCK_SIZE;
    if (mouse_pos.x + (float)offsx < 0.0f)
      tile_x--;
    if (mouse_pos.y + (float)offsy < 0.0f)
      tile_y--;
    return pair<int, int>{tile_x, tile_y};
  }

  [[nodiscard]] bool mouse_in_max_frame() const {
    pair<int, int> mouse_tile_coord = tile_coord();

    return mouse_tile_coord.first >= 0 && mouse_tile_coord.second >= 0 &&
           mouse_tile_coord.first < map_width &&
           mouse_tile_coord.second < map_height;
  }

  static void draw_tile(TileType type, Vector2 pos) {
    Rectangle frame{pos.x, pos.y, BLOCK_SIZE, BLOCK_SIZE};
    Color color;

    switch (type) {
      case TILE_NULL:
        color = RAYWHITE;
        break;
      case TILE_AIR:
        color = LIGHTGRAY;
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
      case TILE_TRAP:
        color = PURPLE;
        break;
      case TILE_SHIELD:
        color = VIOLET;
        break;
      default:
        break;
    }
    DrawRectangleRec(frame, color);
  }

  [[nodiscard]] static int tile_count() {
    return sizeof(tile_types) / sizeof(TileType);
  }

  void save_map() {
    ofstream map_file;
    map_file.open(input_map_file_name.value,
                  ofstream::out | ofstream::in | ofstream::trunc);

    if (!map_file.is_open()) {
      fprintf(stderr, "Cannot write to file");
      return;
    }

    map_file << map_height << endl;

    // Tiles.
    for (int y = 0; y < map_height; y++) {
      for (int x = 0; x < map_width; x++) {
        map_file.put((char)(map[y][x].type == TileType::TILE_NULL
                                ? TileType::TILE_AIR
                                : map[y][x].type));
      }
      map_file.put('\n');
    }

    // Values.
    for (int y = 0; y < map_height; y++) {
      for (int x = 0; x < map_width; x++) {
        if (!map[y][x].pattern.empty()) {
          map_file << "pattern," << x << "," << y << "," << map[y][x].pattern
                   << endl;
        }
        if (map[y][x].decoration >= 0) {
          map_file << "decoration," << x << "," << y << ","
                   << map[y][x].decoration << endl;
        }
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
