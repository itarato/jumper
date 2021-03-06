#pragma once

#include <map>
#include <memory>

#include "asset_manager.h"
#include "game_config.h"
#include "shared/types.h"
#include "stage_menu.h"

struct App {
  std::map<StageT, IStage*> stages;
  GameConfig game_config;

  explicit App(GameConfig&& game_config);

  ~App();

  void init();
  void loop();
};
