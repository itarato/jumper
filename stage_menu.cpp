#include "stage_menu.h"

#include "raylib.h"
#include "util.h"

#define BUTTON_START "Start"
#define BUTTON_TEXT_SIZE 64

void StageMenu::update() {
  if (IsMouseButtonPressed(0)) {
    Vector2 mouse_pos = GetMousePosition();
    if (CheckCollisionPointRec(mouse_pos, start_text.frame())) {
      is_start = true;
    }
  }
}

void StageMenu::draw() { start_text.draw(); }

std::optional<StageT> StageMenu::next_stage() {
  if (is_start) {
    return std::optional<StageT>{STAGE_GAME};
  } else {
    return std::nullopt;
  }
}

void StageMenu::init() {
  start_text.with_aligned_center()->with_hover()->with_color(DARKBLUE);
  is_start = false;
}
