#include "stage_game.h"

#include <algorithm>
#include <cmath>
#include <memory>

#include "asset_manager.h"
#include "defines.h"
#include "input.h"
#include "shared/util.h"

void StageGame::update() {
  if (state == GAME_STATE_PLAY) {
    // Reset stage.
    if (is_key_pressed(KEY_BACKSPACE)) {
      init_level();
      return;
    }

    // Player update.
    jumper.update(&map);

    // Enemy movement.
    for (Enemy& enemy : enemies) {
      if (std::any_of(poops.begin(), poops.end(), [&](const Poop& poop) {
            return CheckCollisionRecs(poop.frame(), enemy.frame);
          })) {
        enemy.paralyzed.set();
        continue;
      }
      enemy.update(jumper.frame);
    }

    for (auto& poop : poops)
      poop.update();

    poops.erase(std::remove_if(poops.begin(), poops.end(),
                               [](const auto& e) { return e.is_dead(); }),
                poops.end());

    {  // Particles
      for (auto& explosion : explosions)
        explosion->update();

      // Cleanup completed explosions.
      explosions.erase(
          std::remove_if(explosions.begin(), explosions.end(),
                         [](const auto& e) { return e->is_completed(); }),
          explosions.end());
    }
  }

  {  // Coin collection.
    for (auto& coin : coins) {
      if (CheckCollisionRecs(coin.frame, jumper.frame)) {
        coin.is_collected = true;
        score++;

        explosions.push_back(
            std::make_unique<Explosion>(jumper.frame, 16, BEIGE));
      }
    }

    coins.erase(std::remove_if(coins.begin(), coins.end(),
                               [](const auto& e) { return e.is_collected; }),
                coins.end());
  }

  {  // Shield collection.
    for (auto& shield : shields) {
      if (CheckCollisionRecs(shield.frame, jumper.frame)) {
        shield.is_collected = true;
        jumper.activate_shield();

        explosions.push_back(
            std::make_unique<Explosion>(jumper.frame, 8, MAGENTA));
      }
    }

    // Fixme: delegate these to a helper.
    shields.erase(std::remove_if(shields.begin(), shields.end(),
                                 [](const auto& e) { return e.is_collected; }),
                  shields.end());
  }

  {  // Completion checks.
    if (state == GAME_STATE_PLAY) {
      if (jumper.frame.y >= (float)map.pixel_height() || jumper.is_dead()) {
        state = GAME_STATE_WAIT_TO_RESTART_LEVEL;
        is_victory = false;
      }

      if (jumper.is_alive()) {
        Rectangle end_rec{map.end_pos.x, map.end_pos.y, BLOCK_SIZE, BLOCK_SIZE};
        if (CheckCollisionRecs(end_rec, jumper.frame)) {
          if (current_map_number < (int)map_file_paths.size() - 1) {
            state = GAME_STATE_WAIT_TO_NEXT_LEVEL;
          } else {
            state = GAME_STATE_WAIT_TO_COMPLETE;
          }
          is_victory = true;
          timer.stop();
        }

        if (!jumper.is_shielded()) {
          for (Enemy& enemy : enemies) {
            if (CheckCollisionRecs(jumper.frame, enemy.frame)) {
              jumper.kill();
            }
          }
        }
      }
    }

    if (state == GAME_STATE_WAIT_TO_COMPLETE ||
        state == GAME_STATE_WAIT_TO_NEXT_LEVEL ||
        state == GAME_STATE_WAIT_TO_RESTART_LEVEL) {
      wait_to_state_timeout++;

      if (wait_to_state_timeout >= WAIT_TO_COMPLETE_FRAMES ||
          is_key_pressed(KEY_ENTER)) {
        if (state == GAME_STATE_WAIT_TO_COMPLETE) {
          state = GAME_STATE_COMPLETE;
        } else if (state == GAME_STATE_WAIT_TO_NEXT_LEVEL) {
          // FIXME - encapsulate state changes with logic.
          state = GAME_STATE_PLAY;
          current_map_number++;
          init_level();
        } else if (state == GAME_STATE_WAIT_TO_RESTART_LEVEL) {
          state = GAME_STATE_PLAY;
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
  IntVector2D scroll_offset{0, 0};
  int h_offs_padding = (GetScreenHeight() - BLOCK_SIZE) / 2;
  int v_offs_padding = (GetScreenHeight() - BLOCK_SIZE) / 2;
  if ((int)map.pixel_width() <= GetScreenWidth()) {
    scroll_offset.x = -(GetScreenWidth() - (int)map.pixel_width()) / 2;
  } else if (jumper.frame.x >= h_offs_padding) {
    scroll_offset.x = std::min((int)map.pixel_width() - GetScreenWidth(),
                               (int)jumper.frame.x - h_offs_padding);
  }
  if ((int)map.pixel_height() <= GetScreenHeight()) {
    scroll_offset.y = -(GetScreenHeight() - (int)map.pixel_height()) / 2;
  } else if (jumper.frame.y >= v_offs_padding) {
    scroll_offset.y = std::min((int)map.pixel_height() - GetScreenHeight(),
                               (int)jumper.frame.y - v_offs_padding);
  }

  // Full background color.
  DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), DARKGRAY);
  // Play area background color.
  DrawRectangle(-scroll_offset.x, -scroll_offset.y, map.pixel_width(),
                map.pixel_height(), GetColor(game_config->background_color()));
  if (game_config->is_background_image()) {
    // Play area tile.
    DrawTextureTiled(
        asset_manager.textures[IMG_BACKGROUND],
        Rectangle{0.0f, 0.0f,
                  (float)asset_manager.textures[IMG_BACKGROUND].width,
                  (float)asset_manager.textures[IMG_BACKGROUND].height},
        Rectangle{
            (float)-scroll_offset.x,
            (float)(map.pixel_height() -
                    std::min(
                        (float)map.pixel_height(),
                        (float)asset_manager.textures[IMG_BACKGROUND].height) -
                    scroll_offset.y),
            (float)map.pixel_width(),
            std::min((float)map.pixel_height(),
                     (float)asset_manager.textures[IMG_BACKGROUND].height)},
        Vector2{0.0f, 0.0f}, 0.0f, 1.0f, WHITE);
  }

  map.draw(scroll_offset);
  jumper.draw(scroll_offset);

  for (const auto& coin : coins)
    coin.draw(scroll_offset);
  for (const auto& poop : poops)
    poop.draw(scroll_offset);
  for (auto& enemy : enemies)
    enemy.draw(scroll_offset);
  for (auto& shield : shields)
    shield.draw(scroll_offset);

  {  // Particles
    for (auto& explosion : explosions)
      explosion->draw(scroll_offset);
  }

  {  // Overlay.
    DrawRectangle(0, GetScreenHeight() - 32, GetScreenWidth(), 32,
                  Fade(BLACK, 0.7f));

    DrawTextEx(asset_manager.fonts[FONT_MEDIUM], TextFormat("Score: %d", score),
               Vector2{12.0f, (float)GetScreenHeight() - 26.0f},
               asset_manager.fonts[FONT_MEDIUM].baseSize, 0, WHITE);

    const char* regex_text = TextFormat("/%s/", jumper.regex_raw.c_str());
    Vector2 regex_text_size{
        MeasureTextEx(asset_manager.fonts[FONT_MEDIUM], regex_text,
                      asset_manager.fonts[FONT_MEDIUM].baseSize, 0)};
    DrawTextEx(asset_manager.fonts[FONT_MEDIUM], regex_text,
               Vector2{((float)GetScreenWidth() - regex_text_size.x) / 2.0f,
                       (float)GetScreenHeight() - 26.0f},
               asset_manager.fonts[FONT_MEDIUM].baseSize, 0, WHITE);

    const char* time_text =
        TextFormat("%dm %ds", timer.minutes(), timer.seconds());
    Vector2 time_text_size{
        MeasureTextEx(asset_manager.fonts[FONT_MEDIUM], time_text,
                      asset_manager.fonts[FONT_MEDIUM].baseSize, 0)};
    DrawTextEx(asset_manager.fonts[FONT_MEDIUM], time_text,
               Vector2{(float)GetScreenWidth() - time_text_size.x - 12.0f,
                       (float)GetScreenHeight() - 26.0f},
               asset_manager.fonts[FONT_MEDIUM].baseSize, 0, WHITE);
  }

  if (state == GAME_STATE_WAIT_TO_COMPLETE ||
      state == GAME_STATE_WAIT_TO_NEXT_LEVEL ||
      state == GAME_STATE_WAIT_TO_RESTART_LEVEL) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                  Fade(RAYWHITE, 0.6));
    if (is_victory) {
      victory_text.draw();
    } else {
      game_over_text.draw();
    }
  }
}

void StageGame::init() {
  LOG_INFO("GAME STAGE INIT");

  current_map_number = 0;

  victory_text.with_font(&asset_manager.fonts[FONT_LARGE])
      ->with_aligned_center();
  game_over_text.with_font(&asset_manager.fonts[FONT_LARGE])
      ->with_aligned_center();

  init_level();
}

void StageGame::init_level() {
  score = 0;

  map.build(map_file_paths[current_map_number]);

  state = GAME_STATE_PLAY;
  jumper.init(map.start_pos);
  wait_to_state_timeout = 0;

  enemies.clear();
  coins.clear();
  explosions.clear();
  poops.clear();
  shields.clear();

  auto random_enemies = map.coords_of_tile_type(TILE_ENEMY_RANDOM);
  for (auto enemy_block_coord : random_enemies) {
    enemies.emplace_back(Rectangle{(float)(enemy_block_coord.x * BLOCK_SIZE),
                                   (float)(enemy_block_coord.y * BLOCK_SIZE),
                                   (float)BLOCK_SIZE, (float)BLOCK_SIZE},
                         std::make_unique<RandomWalker>(&map),
                         &asset_manager.textures[IMG_ENEMY]);
  }

  auto chaser_enemies = map.coords_of_tile_type(TILE_ENEMY_CHASER);
  for (auto enemy_block_coord : chaser_enemies) {
    enemies.emplace_back(Rectangle{(float)(enemy_block_coord.x * BLOCK_SIZE),
                                   (float)(enemy_block_coord.y * BLOCK_SIZE),
                                   (float)BLOCK_SIZE, (float)BLOCK_SIZE},
                         std::make_unique<StrictPathChaseWalker>(&map),
                         &asset_manager.textures[IMG_ENEMY_CHASER]);
  }

  for (Enemy& enemy : enemies)
    enemy.init();

  auto coin_coords = map.coords_of_tile_type(TILE_COIN);
  for (auto coin_coord : coin_coords) {
    coins.emplace_back(Vector2{(float)(coin_coord.x * BLOCK_SIZE),
                               (float)(coin_coord.y * BLOCK_SIZE)});
  }

  auto shield_coords = map.coords_of_tile_type(TILE_SHIELD);
  for (auto shield_coord : shield_coords) {
    shields.emplace_back(Vector2{(float)(shield_coord.x * BLOCK_SIZE),
                                 (float)(shield_coord.y * BLOCK_SIZE)});
  }

  timer.start();
}

std::optional<StageT> StageGame::next_stage() {
  if (state == GAME_STATE_COMPLETE) {
    return std::optional<StageT>{STAGE_MENU};
  } else {
    return std::nullopt;
  }
}

void StageGame::on_jumper_update(JumperEvent event, JumperEventData data) {
  if (event == JumperEvent::CaptureRegex) {
    explosions.push_back(
        std::make_unique<Explosion>(*data.frame, 12, DARKGRAY));
  } else if (event == JumperEvent::Jump) {
    explosions.push_back(std::make_unique<Smoker>(data.frame, LIGHTGRAY));
  } else if (event == JumperEvent::Dash) {
    explosions.push_back(std::make_unique<Smoker>(data.frame, ORANGE));
  } else if (event == JumperEvent::Die) {
    explosions.push_back(std::make_unique<Repeater>(
        *data.frame, 1, 4, 48, [](Rectangle start_frame) {
          return (IParticle*)(new Explosion(start_frame, 8, RED));
        }));
    timer.stop();
  } else if (event == JumperEvent::OpenDoor) {
    explosions.push_back(std::make_unique<Repeater>(
        data.subject, 1, 3, 6, [](Rectangle start_frame) {
          return (IParticle*)(new Sprinkler(position_of(start_frame),
                                            PI * -0.75f, 14, DARKPURPLE));
        }));
  } else if (event == JumperEvent::FailedDoor) {
    explosions.push_back(std::make_unique<Sprinkler>(
        absolute_midpoint(*data.frame, data.subject), 0.0f, 8, RED));
    explosions.push_back(std::make_unique<Sprinkler>(
        absolute_midpoint(*data.frame, data.subject), PI, 8, RED));
  } else if (event == JumperEvent::Poop) {
    poops.emplace_back(Vector2{
        jumper.frame.x,
        (float)((int)(jumper.frame.y / BLOCK_SIZE) * BLOCK_SIZE),
    });
  }
}
