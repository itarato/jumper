#include "app.h"
#include "raylib.h"
#include "stage_game.h"
#include "stage_menu.h"
#include "util.h"

void App::init() {
  current_stage = STAGE_MENU;

  stages.insert({STAGE_MENU, new StageMenu(&game_config)});
  stages.insert({STAGE_GAME, new StageGame(&game_config)});

  InitWindow(800, 800, "Jumper");
  SetTargetFPS(60);
}

void App::loop() {
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
