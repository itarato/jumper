#pragma once

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

  void init(Rectangle &frame);
  void update(Rectangle &frame);

  virtual void set_next_target() {}
};

struct RandomWalker : TargetWalker {
  void set_next_target();
};

struct StrictPathChaseWalker : IWalker {
  Vector2 origin;
  Vector2 target;
  int step;

  void init(Rectangle &frame);
  void update(Rectangle &frame);

  void set_next_target();
};
