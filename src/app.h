#pragma once

#include <map>
#include <memory>

#include "asset_manager.h"
#include "game_config.h"
#include "stage_menu.h"
#include "types.h"

struct App {
  std::map<StageT, IStage *> stages;
  GameConfig game_config;

  App();

  ~App();

  void init();
  void loop();
};
