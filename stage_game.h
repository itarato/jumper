#pragma once

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
  GAME_STATE_COMPLETE = 2,
} GameStateT;

struct DoubleJump {
  Map *map;
  Jumper *jumper;
  int air_jumps;

  DoubleJump(Map *map, Jumper *jumper)
      : map(map), jumper(jumper), air_jumps(0) {}

  bool can_jump() {
    if (jumper->map_state.type == MAP_OBJECT_VERTICAL_STATE_ON_FLOOR) {
      air_jumps = 0;
      return true;
    }

    if (air_jumps >= 1) {
      return false;
    }

    air_jumps++;
    return true;
  }

  void reset() { air_jumps = 0; }
};

struct StageGame : public IStage {
  GameStateT state;
  int wait_to_complete_timeout;
  bool is_victory;

  void update();
  void draw();
  void init();
  std::optional<StageT> next_stage();

  GameConfig *game_config;
  Jumper jumper;
  Map map;
  Enemy enemy;

  DoubleJump double_jump;

  Text victory_text;
  Text game_over_text;

  StageGame(GameConfig *game_config)
      : game_config(game_config),
        map("./maps/1.jm"),
        double_jump(&map, &jumper),
        victory_text("Victory"),
        game_over_text("Game over"){};
  ~StageGame(){};
};
