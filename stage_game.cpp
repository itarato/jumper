#include "stage_game.h"

void StageGame::update() {}

void StageGame::draw() {}

void StageGame::init() { is_over = false; }

std::optional<StageT> StageGame::next_stage() {
  if (is_over) {
    return std::optional<StageT>{STAGE_MENU};
  } else {
    return std::nullopt;
  }
}
