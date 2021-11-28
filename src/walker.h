#pragma once

#include "map.h"
#include "raylib.h"
#include "shared/util.h"

#define RANDOM_WALKER_STEP_COUNT 16

struct IWalker {
  virtual void init(Rectangle &self_frame) = 0;
  virtual void update(Rectangle &self_frame, const Rectangle &player_frame) = 0;
  virtual ~IWalker() = default;
};

struct TargetWalker : IWalker {
  Vector2 origin;
  Vector2 target;
  int step;
  Map *map;

  void init(Rectangle &self_frame) override;
  void update(Rectangle &self_frame, const Rectangle &player_frame) override;

  virtual void set_next_target(Rectangle &self_frame,
                               const Rectangle &player_frame) {}

  explicit TargetWalker(Map *map) : map(map) {}
  ~TargetWalker() override = default;
};

struct RandomWalker : TargetWalker {
  void set_next_target(Rectangle &self_frame, const Rectangle &player_frame) override;

  explicit RandomWalker(Map *map) : TargetWalker(map) {}
};

struct StrictPathChaseWalker : TargetWalker {
  void set_next_target(Rectangle &self_frame, const Rectangle &player_frame) override;

  explicit StrictPathChaseWalker(Map *map) : TargetWalker(map) {}
};
