#include "walker.h"

#include <algorithm>
#include <map>
#include <set>
#include <vector>

#include "types.h"
#include "util.h"

void TargetWalker::init(Rectangle &self_frame) {
  self_frame = Rectangle{
      (float)BLOCK_SIZE * 13.0f,
      (float)BLOCK_SIZE * 9.0f,
      (float)BLOCK_SIZE,
      (float)BLOCK_SIZE,
  };
  origin = Vector2{(float)BLOCK_SIZE * 13.0f, (float)BLOCK_SIZE * 9.0f};
  target = Vector2{(float)BLOCK_SIZE * 13.0f, (float)BLOCK_SIZE * 8.0f};
}

void TargetWalker::update(Rectangle &self_frame,
                          const Rectangle &player_frame) {
  if (step >= RANDOM_WALKER_STEP_COUNT) {
    self_frame.x = target.x;
    self_frame.y = target.y;

    step = 0;
    origin = target;

    set_next_target(self_frame, player_frame);
  }

  step++;

  float fragment_multiplier = (float)step / (float)RANDOM_WALKER_STEP_COUNT;
  self_frame.x = origin.x + ((target.x - origin.x) * fragment_multiplier);
  self_frame.y = origin.y + ((target.y - origin.y) * fragment_multiplier);
}

void RandomWalker::set_next_target(Rectangle &self_frame,
                                   const Rectangle &player_frame) {
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

void StrictPathChaseWalker::set_next_target(Rectangle &self_frame,
                                            const Rectangle &player_frame) {
  IntVector2D start_p{(int)(player_frame.x / BLOCK_SIZE),
                      (int)(player_frame.y / BLOCK_SIZE)};
  IntVector2D end_p{(int)(target.x / BLOCK_SIZE), (int)(target.y / BLOCK_SIZE)};

  AStarNode start{
      start_p,
      0,
      start_p.dist(end_p),
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

      if (neighbour_p == end_p) {
        target.x += BLOCK_SIZE * -dirs[i].x;
        target.y += BLOCK_SIZE * -dirs[i].y;

        return;
      }

      // Out of map.
      if (neighbour_p.x < 0 || neighbour_p.y < 0 ||
          neighbour_p.y >= WINDOW_BLOCK_HEIGHT ||
          neighbour_p.x >= map->block_width) {
        continue;
      }

      // Steppable map tile.
      if (map->is_steppable(neighbour_p.y, neighbour_p.x)) {
        continue;
      }

      auto existing_node_it =
          std::find(visited.begin(), visited.end(), neighbour_p);

      // Already visited.
      if (existing_node_it != visited.end()) continue;
      visited.insert(neighbour_p);

      AStarNode neighbour_node{
          neighbour_p,
          node.pre_cost + 1,
          neighbour_p.dist(end_p),
      };
      inspected.push_back(neighbour_node);
    }
  }
}
