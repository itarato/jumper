#pragma once

#include <optional>

#include "raylib.h"

typedef enum {
  STAGE_MENU = 0,
  STAGE_GAME = 1,
} StageT;

struct IStage {
  virtual void update() = 0;
  virtual void draw() = 0;
  virtual void init() = 0;
  virtual std::optional<StageT> next_stage() = 0;
  virtual ~IStage(){};
};
