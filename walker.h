#pragma once

#include "map.h"
#include "raylib.h"
#include "util.h"

#define RANDOM_WALKER_STEP_COUNT 16

struct IWalker {
  virtual void init(Rectangle &self_frame) = 0;
  virtual void update(Rectangle &self_frame, const Rectangle &player_frame) = 0;
  virtual ~IWalker(){};
};

struct TargetWalker : IWalker {
  Vector2 origin;
  Vector2 target;
  int step;
  Map *map;

  void init(Rectangle &self_frame);
  void update(Rectangle &self_frame, const Rectangle &player_frame);

  virtual void set_next_target(Rectangle &self_frame,
                               const Rectangle &player_frame) {}

  TargetWalker(Map *map) : map(map) {}
  ~TargetWalker() {}
};

struct RandomWalker : TargetWalker {
  void set_next_target(Rectangle &self_frame, const Rectangle &player_frame);

  RandomWalker(Map *map) : TargetWalker(map) {}
  ~RandomWalker() { LOG_INFO("RandomWalker dtor"); }
};

struct StrictPathChaseWalker : TargetWalker {
  void set_next_target(Rectangle &self_frame, const Rectangle &player_frame);

  StrictPathChaseWalker(Map *map) : TargetWalker(map) {}
};
