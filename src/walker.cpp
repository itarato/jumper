#include "walker.h"

#include <algorithm>
#include <cstring>
#include <vector>

#include "defines.h"
#include "shared/types.h"
#include "shared/util.h"

void TargetWalker::init(Rectangle& self_frame) {
  step = RANDOM_WALKER_STEP_COUNT;
  origin = Vector2{self_frame.x, self_frame.y};
  target = Vector2{self_frame.x, self_frame.y};
}

void TargetWalker::update(Rectangle& self_frame,
                          const Rectangle& player_frame) {
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

void RandomWalker::set_next_target(Rectangle& self_frame,
                                   const Rectangle& player_frame) {
  Vector2 next_target;

  for (int i = 0; i < 4; i++) {
    // 4 tries to find a viable next option - not to waste too much and keep it
    // stupid.

    next_target = target;
    switch (rand_range(0, 3)) {
      case 0:
        next_target.x = target.x + BLOCK_SIZE;
        break;
      case 1:
        next_target.x = target.x - BLOCK_SIZE;
        break;
      case 2:
        next_target.y = target.y + BLOCK_SIZE;
        break;
      case 3:
        next_target.y = target.y - BLOCK_SIZE;
        break;
    }

    IntVector2D coord{(int)next_target.x / BLOCK_SIZE,
                      (int)next_target.y / BLOCK_SIZE};
    if (map->is_inside_map(coord) && !map->is_solid_tile(coord)) {
      target = next_target;
      break;
    }
  }
}

void StrictPathChaseWalker::set_next_target(Rectangle& self_frame,
                                            const Rectangle& player_frame) {
  //  Timer dbg_timer{};
  //  dbg_timer.tick();
  //  Ticker dbg_ticker{};

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

  const IntVector2D dirs[4]{
      {-1, 0},
      {1, 0},
      {0, -1},
      {0, 1},
  };

  // Using an array for const time find() performance.
  auto* visited = (uint8_t*)malloc(sizeof(uint8_t) *
                                   (map->pixel_height() * map->pixel_width()));
  memset(visited, 0,
         sizeof(uint8_t) * (map->pixel_height() * map->pixel_width()));

  while (!inspected.empty()) {
    //    dbg_ticker.tick();
    auto closest_it =
        std::min_element(inspected.begin(), inspected.end(),
                         [](AStarNode const& lhs, AStarNode const& rhs) {
                           return (lhs.pre_cost + lhs.estimate_to_goal) <
                                  (rhs.pre_cost + rhs.estimate_to_goal);
                         });
    if (closest_it == inspected.end()) {
      PANIC("Cannot find closest element.");
    }

    AStarNode node = *closest_it;
    inspected.erase(closest_it);

    for (auto dir : dirs) {
      const IntVector2D neighbour_p{node.p.x + dir.x, node.p.y + dir.y};

      if (neighbour_p == end_p) {
        target.x += BLOCK_SIZE * -dir.x;
        target.y += BLOCK_SIZE * -dir.y;

        //        dbg_timer.tock_and_dump("A* (success)");
        //                dbg_ticker.dump("A* (success)");

        free(visited);

        return;
      }

      // Out of map or steppable map tile.
      if (!map->is_inside_map(neighbour_p) || map->is_solid_tile(neighbour_p)) {
        continue;
      }

      // Skip if already visited.
      if (visited[neighbour_p.y * map->pixel_width() + neighbour_p.x] == 1)
        continue;
      visited[neighbour_p.y * map->pixel_width() + neighbour_p.x] = 1;

      AStarNode new_node{neighbour_p, node.pre_cost + 1,
                             neighbour_p.dist(end_p)};
      inspected.push_back(new_node);
    }
  }

  //  dbg_timer.tock_and_dump("A* (fail)");
  //  dbg_ticker.dump("A* (fail)");

  free(visited);
}
