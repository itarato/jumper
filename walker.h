#pragma once

#include "map.h"
#include "raylib.h"

#define RANDOM_WALKER_STEP_COUNT 4

struct IWalker {
  virtual void init(Rectangle &frame) = 0;
  virtual void update(Rectangle &frame) = 0;
  virtual ~IWalker(){};
};

struct TargetWalker : IWalker {
  Vector2 origin;
  Vector2 target;
  int step;
  Map *map;

  void init(Rectangle &frame);
  void update(Rectangle &frame);

  virtual void set_next_target(Rectangle &frame) {}

  TargetWalker(Map *map) : map(map) {}
};

struct RandomWalker : TargetWalker {
  void set_next_target(Rectangle &frame);

  RandomWalker(Map *map) : TargetWalker(map) {}
};

struct StrictPathChaseWalker : TargetWalker {
  void set_next_target(Rectangle &frame);

  StrictPathChaseWalker(Map *map) : TargetWalker(map) {}
};
