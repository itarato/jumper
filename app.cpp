#include "app.h"

#include "asset_manager.h"
#include "raylib.h"
#include "stage_game.h"
#include "stage_menu.h"

void App::init() {
  stages.insert({STAGE_MENU, new StageMenu(&game_config)});
  stages.insert({STAGE_GAME, new StageGame(&game_config)});

  current_stage = STAGE_MENU;

  InitWindow(WINDOW_WITDH, WINDOW_HEIGH, "Jumper");
  SetTargetFPS(60);

  std::vector<std::string> images{IMG_GROUND, IMG_BACKGROUND, IMG_ENEMY,
                                  IMG_COIN, IMG_LADYBUG};
  for (auto& image : images) {
    asset_manager.textures.insert({image, LoadTexture(image.c_str())});
  }
}

void App::loop() {
  stages[current_stage]->init();

  while (!WindowShouldClose()) {
    auto stage = stages[current_stage];

    stage->update();

    BeginDrawing();
    ClearBackground(RAYWHITE);

    stage->draw();
    DrawFPS(GetScreenWidth() - 96, 12);

    auto next_stage = stage->next_stage();
    if (next_stage.has_value()) {
      LOG_INFO("Stage change: %d -> %d", current_stage, next_stage.value());
      current_stage = next_stage.value();
      stages[current_stage]->init();
    }

    EndDrawing();
  }
}
