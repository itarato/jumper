#pragma once

#include <memory>

#include "asset_manager.h"
#include "coin.h"
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

static const char* game_map_files[]{
    "/maps/1.jm",
    "/maps/2.jm",
    "/maps/3.jm",
};

typedef enum {
  GAME_STATE_PLAY = 0,
  GAME_STATE_WAIT_TO_COMPLETE = 1,
  GAME_STATE_WAIT_TO_NEXT_LEVEL = 2,
  GAME_STATE_WAIT_TO_RESTART_LEVEL = 3,
  GAME_STATE_COMPLETE = 4,
} GameStateT;

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
  int score;

  Text victory_text;
  Text game_over_text;

  int current_map_number;
  std::vector<std::string> map_file_paths;
  std::vector<std::unique_ptr<IParticle>> explosions{};

  SimpleTimer timer{};

  void on_jumper_update(JumperEvent event, JumperEventData data) override;

  void update() override;
  void draw() override;
  void init() override;
  void init_level();
  std::optional<StageT> next_stage() override;

  explicit StageGame(GameConfig* game_config)
      : JumperObserver(),
        game_config(game_config),
        victory_text("Victory"),
        game_over_text("Game over"),
        map_file_paths({}) {
    jumper.JumperSubject::subscribe(this);

    auto pre_selected_map = game_config->selected_map();
    if (pre_selected_map.has_value()) {
      map_file_paths.push_back(pre_selected_map.value());
    } else {
      for (const auto& file : game_map_files) {
        map_file_paths.emplace_back(concat(game_config->resource_dir(), file));
      }
    }
  };
  ~StageGame() override = default;
};
