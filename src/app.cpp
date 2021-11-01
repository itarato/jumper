#include "app.h"

#include <ranges>

#include "asset_manager.h"
#include "defines.h"
#include "raylib.h"
#include "stage_game.h"
#include "stage_menu.h"

App::App() = default;

App::~App() {
  for (auto& stage : stages | std::views::values) {
    delete stage;
    stage = nullptr;
  }
}

void App::init() {
  stages.insert({STAGE_MENU, new StageMenu(&game_config)});
  stages.insert({STAGE_GAME, new StageGame(&game_config)});

  InitWindow(320, 320, "Jumper");
  SetTargetFPS(60);

  std::vector<std::string> images{IMG_GROUND,
                                  IMG_GROUND_TOP,
                                  IMG_BACKGROUND,
                                  IMG_ENEMY,
                                  IMG_COIN,
                                  IMG_LADYBUG_STAND_0,
                                  IMG_LADYBUG_STAND_1,
                                  IMG_LADYBUG_STAND_2,
                                  IMG_LADYBUG_STAND_3,
                                  IMG_LADYBUG_MOVE_0,
                                  IMG_LADYBUG_MOVE_1,
                                  IMG_LADYBUG_MOVE_2,
                                  IMG_LADYBUG_MOVE_3,
                                  IMG_DOOR_OPEN,
                                  IMG_DOOR_CLOSE,
                                  IMG_REGEX};

  for (auto& image : images) {
    asset_manager.textures.insert({image, LoadTexture(image.c_str())});
  }
}

void App::loop() {
  StageT current_stage = STAGE_MENU;
  stages[current_stage]->init();

  while (!WindowShouldClose()) {
    auto stage = stages[current_stage];

    stage->update();

    BeginDrawing();
    ClearBackground(RAYWHITE);

    stage->draw();
    DrawFPS(GetScreenWidth() - 96, GetScreenHeight() - 26);

    auto next_stage = stage->next_stage();
    if (next_stage.has_value()) {
      LOG_INFO("Stage change: %d -> %d", current_stage, next_stage.value());
      current_stage = next_stage.value();
      stages[current_stage]->init();
    }

    EndDrawing();
  }
}
