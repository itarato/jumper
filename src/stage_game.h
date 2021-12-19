#pragma once

#include <memory>

#include "asset_manager.h"
#include "collectible.h"
#include "enemy.h"
#include "game_config.h"
#include "jumper.h"
#include "map.h"
#include "particles.h"
#include "poop.h"
#include "shared/types.h"
#include "shared/util.h"
#include "text.h"

#define WAIT_TO_COMPLETE_FRAMES 180

enum class GameStateT {
  PLAY = 0,
  WAIT_TO_COMPLETE = 1,
  WAIT_TO_NEXT_LEVEL = 2,
  WAIT_TO_RESTART_LEVEL = 3,
  COMPLETE = 4,
};

struct StageGame : IStage, JumperObserver {
  GameStateT state;
  int wait_to_state_timeout;
  bool is_victory;

  GameConfig* game_config;
  Jumper jumper;
  Map map;
  std::vector<Enemy> enemies;
  std::vector<Coin> coins;
  std::vector<Poop> poops;
  std::vector<Shield> shields;
  int score;

  Text victory_text;
  Text game_over_text;
  Text game_over_explanation_text;
  Text pause_text;

  int current_map_number;
  std::vector<std::string> map_file_paths;
  std::vector<std::unique_ptr<IParticle>> explosions{};

  SimpleTimer timer{};

  bool is_paused{false};

  void on_jumper_update(JumperEvent event, JumperEventData data) override;

  void update() override;
  void draw() override;
  void init() override;
  void init_level();
  std::optional<StageT> next_stage() override;

  explicit StageGame(GameConfig* game_config);
  ~StageGame() override = default;
};
