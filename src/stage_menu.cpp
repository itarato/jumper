#include "stage_menu.h"

#include "asset_manager.h"
#include "defines.h"
#include "input.h"
#include "raylib.h"

void StageMenu::update() {
  if ((IsMouseButtonPressed(0) &&
       CheckCollisionPointRec(GetMousePosition(), start_text.frame())) ||
      is_key_pressed(KEY_ENTER)) {
    TraceLog(LOG_ERROR, "Stage change menu to game");
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
  start_text.with_font(&asset_manager.fonts[FONT_MEDIUM])->with_aligned_center()->with_hover()->with_color(DARKGRAY);
  is_start = false;
}
