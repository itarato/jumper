#pragma once

#include "game_config.h"
#include "stage_menu.h"
#include "types.h"
#include <map>

struct App {
  std::map<StageT, IStage *> stages;

  StageT current_stage;

  GameConfig game_config;

  App(){};

  ~App() {
    for (auto &[_, stage] : stages) {
      delete stage;
      stage = nullptr;
    }
  };

  void init();
  void loop();
};