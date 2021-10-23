#include "app.h"

#include "raylib.h"
#include "stage_game.h"
#include "stage_menu.h"
#include "util.h"

void App::init() {
  stages.insert({STAGE_MENU, new StageMenu(&game_config)});
  stages.insert({STAGE_GAME, new StageGame(&game_config)});

  current_stage = STAGE_GAME;

  InitWindow(WINDOW_WITDH, WINDOW_HEIGH, "Jumper");
  SetTargetFPS(60);
}

void App::loop() {
  stages[current_stage]->init();

  while (!WindowShouldClose()) {
    auto stage = stages[current_stage];

    stage->update();

    BeginDrawing();
    ClearBackground(RAYWHITE);

    stage->draw();

    auto next_stage = stage->next_stage();
    if (next_stage.has_value()) {
      LOG_INFO("Stage change: %d -> %d", current_stage, next_stage.value());
      current_stage = next_stage.value();
      stages[current_stage]->init();
    }

    EndDrawing();
  }
}
