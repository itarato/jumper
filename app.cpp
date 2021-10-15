#include "app.h"
#include "raylib.h"
#include "stage_menu.h"

void App::init() {
  current_stage = STAGE_MENU;

  stages.insert({STAGE_MENU, new StageMenu()});

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
      current_stage = next_stage.value();
    }

    EndDrawing();
  }
}
