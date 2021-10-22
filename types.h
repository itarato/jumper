#pragma once

#include <cmath>
#include <optional>

#include "raylib.h"

typedef enum {
  STAGE_MENU = 0,
  STAGE_GAME = 1,
} StageT;

struct IntVector2D {
  int x;
  int y;

  int dist(const IntVector2D other) const {
    return abs(x - other.x) + abs(y - other.y);
  }

  friend bool operator<(IntVector2D const& lhs, IntVector2D const& rhs) {
    return lhs.x == rhs.x ? (lhs.y < rhs.y) : (lhs.x < rhs.x);
  }

  friend constexpr bool operator==(const IntVector2D& lhs,
                                   const IntVector2D& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
  }
};

struct AStarNode {
  IntVector2D p;
  int pre_cost;
  int estimate_to_goal;

  friend bool operator<(AStarNode const& lhs, AStarNode const& rhs) {
    return lhs.p.x == rhs.p.x ? (lhs.p.y < rhs.p.y) : (lhs.p.x < rhs.p.x);
  }
};

struct IStage {
  virtual void update() = 0;
  virtual void draw() = 0;
  virtual void init() = 0;
  virtual std::optional<StageT> next_stage() = 0;
  virtual ~IStage(){};
};
