#pragma once

#include "game_config.h"
#include "shared/types.h"
#include "text.h"

struct StageMenu : public IStage {
  bool is_start{false};

  GameConfig *game_config;

  void update() override;
  void draw() override;
  void init() override;
  std::optional<StageT> next_stage() override;

  Text start_text;

  explicit StageMenu(GameConfig *game_config)
      : game_config(game_config),
        start_text("Start"){};
  ~StageMenu() override = default;
};
