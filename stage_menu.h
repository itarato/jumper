#pragma once

#include "game_config.h"
#include "text.h"
#include "types.h"

struct StageMenu : public IStage {
  bool is_start;

  GameConfig *game_config;

  void update();
  void draw();
  void init();
  std::optional<StageT> next_stage();

  Text start_text;

  StageMenu(GameConfig *game_config)
      : game_config(game_config), start_text("Start"){};
  ~StageMenu(){};
};
