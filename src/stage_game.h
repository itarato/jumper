#pragma once

#include <memory>

#include "asset_manager.h"
#include "coin.h"
#include "enemy.h"
#include "game_config.h"
#include "jumper.h"
#include "map.h"
#include "particles.h"
#include "shared/types.h"
#include "text.h"

#define WAIT_TO_COMPLETE_FRAMES 180

typedef enum {
  GAME_STATE_PLAY = 0,
  GAME_STATE_WAIT_TO_COMPLETE = 1,
  GAME_STATE_WAIT_TO_NEXT_LEVEL = 2,
  GAME_STATE_COMPLETE = 3,
} GameStateT;

struct StageGame : IStage, JumperObserver {
  GameStateT state;
  int wait_to_state_timeout;
  bool is_victory;

  void update() override;
  void draw() override;
  void init() override;
  void init_level();
  std::optional<StageT> next_stage() override;

  GameConfig *game_config;
  Jumper jumper;
  Map map;
  std::vector<Enemy> enemies;
  std::vector<Coin> coins;
  int score;

  Text victory_text;
  Text game_over_text;

  int current_map_number;
  std::vector<std::string> map_file_paths;
  std::vector<std::unique_ptr<IParticle>> explosions{};

  void on_jumper_update(JumperEvent event, JumperEventData data) override;

  StageGame(GameConfig *game_config)
      : JumperObserver(),
        game_config(game_config),
        victory_text("Victory"),
        game_over_text("Game over"),
        map_file_paths({"./maps/1.jm", "./maps/2.jm"}) {
    jumper.JumperSubject::subscribe(this);
  };
  ~StageGame() override = default;
};
