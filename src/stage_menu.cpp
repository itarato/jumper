#include "stage_menu.h"

#include "raylib.h"

void StageMenu::update() {
  if ((IsMouseButtonPressed(0) &&
       CheckCollisionPointRec(GetMousePosition(), start_text.frame())) ||
      IsKeyPressed(KEY_ENTER)) {
    is_start = true;
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
  start_text.with_aligned_center()->with_hover()->with_color(DARKGRAY);
  is_start = false;
}
