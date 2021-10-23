#pragma once

#include <map>
#include <memory>

#include "asset_manager.h"
#include "game_config.h"
#include "stage_menu.h"
#include "types.h"

struct App {
  std::map<StageT, IStage *> stages;
  StageT current_stage;
  GameConfig game_config;

  App() {}

  ~App() {
    for (auto &[_, stage] : stages) {
      delete stage;
      stage = nullptr;
    }

    // Fixme: this can be part of asset manager.
    for (auto &[_, texture] : asset_manager.textures) {
      UnloadTexture(texture);
    }
    asset_manager.textures.clear();
  };

  void init();
  void loop();
};