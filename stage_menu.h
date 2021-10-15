#pragma once

#include "types.h"

struct StageMenu : public IStage {
  void update();
  void draw();
  std::optional<StageT> next_stage();
  ~StageMenu(){};
};
