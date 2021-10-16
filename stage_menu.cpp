#include "stage_menu.h"
#include "raylib.h"

Rectangle start_menu_rect() { return {0, 0, 100, 100}; }

void StageMenu::update() {
  if (IsMouseButtonPressed(0)) {
    Vector2 mouse_pos = GetMousePosition();
    if (CheckCollisionPointRec(mouse_pos, start_menu_rect())) {
      is_start = true;
    }
  }
}

void StageMenu::draw() {
  DrawRectangleRec(start_menu_rect(), BLUE);
  DrawText("Start", 10, 10, 32, DARKGRAY);
}

std::optional<StageT> StageMenu::next_stage() {
  if (is_start) {
    return std::optional<StageT>{STAGE_GAME};
  } else {
    return std::nullopt;
  }
}

void StageMenu::init() { is_start = false; }
