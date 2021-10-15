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
    BeginDrawing();
    ClearBackground(RAYWHITE);
    EndDrawing();
  }
}
