#pragma once

#include "raylib.h"
#include <optional>

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

// FIXME - This could be just a simple non-virtual class owning these.
struct IMovable {
  virtual Vector2 get_pos() const = 0;
  virtual Vector2 get_v() const = 0;
  virtual void move(Vector2 delta) = 0;
};

// FIXME - Unused.
struct Mass {
  float force_g;

  void update(IMovable *movable);
};
