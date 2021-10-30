#pragma once

#include <memory>

#include "asset_manager.h"
#include "coin.h"
#include "enemy.h"
#include "game_config.h"
#include "jumper.h"
#include "map.h"
#include "text.h"
#include "types.h"

#define WAIT_TO_COMPLETE_FRAMES 180

typedef enum {
  GAME_STATE_PLAY = 0,
  GAME_STATE_WAIT_TO_COMPLETE = 1,
  GAME_STATE_WAIT_TO_NEXT_LEVEL = 2,
  GAME_STATE_COMPLETE = 3,
} GameStateT;

struct StageGame : public IStage {
  GameStateT state;
  int wait_to_state_timeout;
  bool is_victory;

  void update();
  void draw();
  void init();
  void init_level();
  std::optional<StageT> next_stage();

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

  StageGame(GameConfig *game_config)
      : game_config(game_config),
        victory_text("Victory"),
        game_over_text("Game over"),
        map_file_paths({"./maps/1.jm", "./maps/2.jm"}){};
  ~StageGame(){};
};
