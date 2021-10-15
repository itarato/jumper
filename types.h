#pragma once

#include <optional>

typedef enum {
  STAGE_MENU = 0,
} StageT;

struct IStage {
  virtual void draw() = 0;
  virtual std::optional<StageT> next_stage() = 0;
  virtual ~IStage(){};
};
