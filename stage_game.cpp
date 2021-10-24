#include "stage_game.h"

#include <algorithm>
#include <cmath>
#include <memory>

#include "util.h"

/**
 * ← / → : move
 * SPACE : jump / double jump
 * ALT   : parachute
 * CTRL  : dash
 */

#define WINDOW_SCROLL_PADDING 256

void StageGame::update() {
  if (state == GAME_STATE_PLAY) {
    jumper.update(&map);

    {  // Enemy movement.
      for (Enemy& enemy : enemies) {
        enemy.update(jumper.frame);
      }
    }
  }

  {  // Coin collection.
    for (auto& coin : coins) {
      if (CheckCollisionRecs(coin.frame, jumper.frame)) {
        coin.is_collected = true;
        score++;
      }
    }

    coins.erase(std::remove_if(coins.begin(), coins.end(),
                               [](const auto& e) { return e.is_collected; }),
                coins.end());
  }

  {  // Completion checks.
    if (state == GAME_STATE_PLAY) {
      if (jumper.frame.y > GetScreenHeight()) {
        state = GAME_STATE_WAIT_TO_COMPLETE;
        is_victory = false;
      }

      Rectangle end_rec{map.end_pos.x, map.end_pos.y, BLOCK_SIZE, BLOCK_SIZE};
      if (CheckCollisionRecs(end_rec, jumper.frame)) {
        if (current_map_number < (int)map_file_paths.size() - 1) {
          state = GAME_STATE_WAIT_TO_NEXT_LEVEL;
        } else {
          state = GAME_STATE_WAIT_TO_COMPLETE;
        }
        is_victory = true;
      }

      for (Enemy& enemy : enemies) {
        if (CheckCollisionRecs(jumper.frame, enemy.frame)) {
          state = GAME_STATE_WAIT_TO_COMPLETE;
          is_victory = false;
        }
      }
    }

    if (state == GAME_STATE_WAIT_TO_COMPLETE ||
        state == GAME_STATE_WAIT_TO_NEXT_LEVEL) {
      wait_to_state_timeout++;

      if (wait_to_state_timeout >= WAIT_TO_COMPLETE_FRAMES ||
          IsKeyPressed(KEY_ENTER)) {
        if (state == GAME_STATE_WAIT_TO_COMPLETE) {
          state = GAME_STATE_COMPLETE;
        } else if (state == GAME_STATE_WAIT_TO_NEXT_LEVEL) {
          // FIXME - encapsulate state changes with logic.
          state = GAME_STATE_PLAY;
          current_map_number++;
          init_level();
        }
      }
    }
  }
}

void StageGame::draw() {
  /**
   * Window scroll calc:
   * Map:    |------------------------------------------------|
   * Window:           |--------------|
   * Offset:  ---------^
   */
  int scroll_offset = 0;
  if (jumper.frame.x >= WINDOW_SCROLL_PADDING) {
    scroll_offset = std::min((map.block_width * BLOCK_SIZE) - GetScreenWidth(),
                             (int)jumper.frame.x - WINDOW_SCROLL_PADDING);
  }

  DrawTextureTiled(
      asset_manager.textures[IMG_BACKGROUND],
      Rectangle{0.0f, 0.0f, 1.0f, (float)GetScreenHeight()},
      Rectangle{0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight()},
      Vector2{0.0f, 0.0f}, 0.0f, 1.1f, WHITE);

  map.draw(scroll_offset);
  jumper.draw(scroll_offset);

  for (Enemy const& enemy : enemies) {
    enemy.draw(scroll_offset);
  }

  for (auto coin : coins) {
    coin.draw(scroll_offset);
  }

  if (state == GAME_STATE_WAIT_TO_COMPLETE ||
      state == GAME_STATE_WAIT_TO_NEXT_LEVEL) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                  Fade(RAYWHITE, 0.6));
    if (is_victory) {
      victory_text.draw();
    } else {
      game_over_text.draw();
    }
  }

  DrawText(TextFormat("Score: %d", score), 12, 12, 20, DARKGRAY);
}

void StageGame::init() {
  LOG_INFO("GAME STAGE INIT");

  current_map_number = 0;

  victory_text.with_font_size(64)->with_aligned_center();
  game_over_text.with_font_size(64)->with_aligned_center();

  init_level();
}

void StageGame::init_level() {
  score = 0;

  map.build(map_file_paths[current_map_number]);

  state = GAME_STATE_PLAY;
  jumper.init(map.start_pos);
  wait_to_state_timeout = 0;

  enemies.clear();

  auto random_enemies = map.coords_of_tile_type(TILE_ENEMY_RANDOM);
  for (auto enemy_block_coord : random_enemies) {
    enemies.emplace_back(Rectangle{(float)(enemy_block_coord.x * BLOCK_SIZE),
                                   (float)(enemy_block_coord.y * BLOCK_SIZE),
                                   (float)BLOCK_SIZE, (float)BLOCK_SIZE},
                         std::make_unique<RandomWalker>(&map));
  }

  auto chaser_enemies = map.coords_of_tile_type(TILE_ENEMY_CHASER);
  for (auto enemy_block_coord : chaser_enemies) {
    enemies.emplace_back(Rectangle{(float)(enemy_block_coord.x * BLOCK_SIZE),
                                   (float)(enemy_block_coord.y * BLOCK_SIZE),
                                   (float)BLOCK_SIZE, (float)BLOCK_SIZE},
                         std::make_unique<StrictPathChaseWalker>(&map));
  }

  for (Enemy& enemy : enemies) enemy.init();

  auto coin_coords = map.coords_of_tile_type(TILE_COIN);
  for (auto coin_coord : coin_coords) {
    coins.emplace_back(Vector2{(float)(coin_coord.x * BLOCK_SIZE),
                               (float)(coin_coord.y * BLOCK_SIZE)});
  }
}

std::optional<StageT> StageGame::next_stage() {
  if (state == GAME_STATE_COMPLETE) {
    return std::optional<StageT>{STAGE_MENU};
  } else {
    return std::nullopt;
  }
}
