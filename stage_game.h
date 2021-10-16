#pragma once

#include "game_config.h"
#include "jumper.h"
#include "map.h"
#include "types.h"

struct StageGame : public IStage {
  bool is_over;

  void update();
  void draw();
  void init();
  std::optional<StageT> next_stage();

  GameConfig *game_config;
  Jumper jumper;
  Map map;

  StageGame(GameConfig *game_config) : game_config(game_config){};
  ~StageGame(){};
};
