#pragma once

#include <cmath>
#include <optional>

#include "raylib.h"

// FIXME: Does not belong here.
typedef enum {
  STAGE_MENU = 0,
  STAGE_GAME = 1,
} StageT;

typedef struct IntVector2D {
  int x;
  int y;

  IntVector2D(int x, int y) : x(x),
                              y(y) {}

  [[nodiscard]] int dist(const IntVector2D other) const {
    return (int) sqrt(pow(x - other.x, 2) + pow(y - other.y, 2));
  }

  friend bool operator<(IntVector2D const& lhs, IntVector2D const& rhs) {
    return lhs.x == rhs.x ? (lhs.y < rhs.y) : (lhs.x < rhs.x);
  }

  friend constexpr bool operator==(const IntVector2D& lhs,
                                   const IntVector2D& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
  }
} IntVector2D;

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
  virtual ~IStage() = default;
};
