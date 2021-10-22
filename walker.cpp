#include "walker.h"

#include <algorithm>
#include <map>
#include <set>
#include <vector>

#include "types.h"
#include "util.h"

void TargetWalker::init(Rectangle &frame) {
  frame = Rectangle{
      (float)BLOCK_SIZE * 13.0f,
      (float)BLOCK_SIZE * 9.0f,
      (float)BLOCK_SIZE,
      (float)BLOCK_SIZE,
  };
  origin = Vector2{(float)BLOCK_SIZE * 13.0f, (float)BLOCK_SIZE * 9.0f};
  target = Vector2{(float)BLOCK_SIZE * 13.0f, (float)BLOCK_SIZE * 8.0f};
}

void TargetWalker::update(Rectangle &frame) {
  if (step >= RANDOM_WALKER_STEP_COUNT) {
    set_next_target(frame);
  }

  step++;

  float fragment_multiplier = (float)step / (float)RANDOM_WALKER_STEP_COUNT;
  frame.x = origin.x + ((target.x - origin.x) * fragment_multiplier);
  frame.y = origin.y + ((target.y - origin.y) * fragment_multiplier);
}

void RandomWalker::set_next_target(Rectangle &frame) {
  step = 0;
  origin = target;

  switch (rand_range(0, 3)) {
    case 0:
      target.x += BLOCK_SIZE;
      break;
    case 1:
      target.x -= BLOCK_SIZE;
      break;
    case 2:
      target.y += BLOCK_SIZE;
      break;
    case 3:
      target.y -= BLOCK_SIZE;
      break;
  }
}

void StrictPathChaseWalker::set_next_target(Rectangle &frame) {
  origin = target;

  IntVector2D end{(int)(frame.x / BLOCK_SIZE), (int)(frame.y / BLOCK_SIZE)};
  AStarNode start{
      {(int)(target.x / BLOCK_SIZE), (int)(target.y / BLOCK_SIZE)},
      0,
      start.p.dist(end),
  };

  std::vector<AStarNode> inspected;
  inspected.push_back(start);

  std::set<IntVector2D> visited;
  visited.insert(start.p);

  const IntVector2D dirs[4]{
      {-1, 0},
      {1, 0},
      {0, -1},
      {0, 1},
  };

  while (!inspected.empty()) {
    auto closest_it =
        std::min_element(inspected.begin(), inspected.end(),
                         [](AStarNode const &lhs, AStarNode const &rhs) {
                           return (lhs.pre_cost + lhs.estimate_to_goal) <
                                  (rhs.pre_cost + rhs.estimate_to_goal);
                         });
    if (closest_it == inspected.end()) {
      fprintf(stderr, "Cannot find closest element.");
      exit(EXIT_FAILURE);
    }

    AStarNode node = *closest_it;
    inspected.erase(closest_it);

    for (int i = 0; i < 4; i++) {
      const IntVector2D neighbour_p{node.p.x + dirs[i].x, node.p.y + dirs[i].y};

      if (neighbour_p.x < 0 || neighbour_p.y < 0 ||
          neighbour_p.y >= WINDOW_BLOCK_HEIGHT ||
          neighbour_p.x >= map->block_width) {
        continue;
      }
      // auto existing_node_it =
      //     std::find(visited.begin(), visited.end(),
      //               [&](IntVector2D const &lhs) { return lhs == neighbour_p;
      //               });

      // AStarNode neighbor_node{
      //     neighbour_p,
      //     node.pre_cost + 1,
      //     neighbour_p.dist(end),
      // };
    }
  }
}
