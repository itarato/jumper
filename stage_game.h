#pragma once

#include "game_config.h"
#include "jumper.h"
#include "map.h"
#include "types.h"

struct DoubleJump {
  Map *map;
  Jumper *jumper;
  int air_jumps;

  DoubleJump(Map *map, Jumper *jumper)
      : map(map), jumper(jumper), air_jumps(0) {}

  bool can_jump() {
    if (jumper->map_state.type == MAP_OBJECT_STATE_TYPE_ON_FLOOR) {
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
  bool is_over;

  void update();
  void draw();
  void init();
  std::optional<StageT> next_stage();

  GameConfig *game_config;
  Jumper jumper;
  Map map;

  DoubleJump double_jump;

  StageGame(GameConfig *game_config)
      : game_config(game_config), double_jump(&map, &jumper){};
  ~StageGame(){};
};
