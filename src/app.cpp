#include "app.h"

#include <ranges>
#include <string>

#include "asset_manager.h"
#include "defines.h"
#include "raylib.h"
#include "stage_game.h"
#include "stage_menu.h"

App::App(std::map<std::string, std::string> argmap)
    : game_config(std::move(argmap)) {}

App::~App() {
  for (auto& stage : stages | std::views::values) {
    delete stage;
    stage = nullptr;
  }
}

void App::init() {
  stages.insert({STAGE_MENU, new StageMenu(&game_config)});
  stages.insert({STAGE_GAME, new StageGame(&game_config)});

  bool is_fullscreen = game_config.is_fullscreen();
  InitWindow(is_fullscreen ? 0 : game_config.window_width(),
             is_fullscreen ? 0 : game_config.window_height(), "Jumper");
  SetTargetFPS(60);

  std::vector<std::string> images{IMG_GROUND,
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
                                  IMG_LADYBUG_FLY_0,
                                  IMG_LADYBUG_FLY_1,
                                  IMG_DOOR_OPEN,
                                  IMG_DOOR_CLOSE,
                                  IMG_DOOR_CLOSE_0,
                                  IMG_DOOR_CLOSE_1,
                                  IMG_DOOR_CLOSE_2,
                                  IMG_DOOR_CLOSE_3,
                                  IMG_DOOR_CLOSE_4,
                                  IMG_DOOR_CLOSE_5,
                                  IMG_DOOR_CLOSE_6,
                                  IMG_DOOR_CLOSE_7,
                                  IMG_REGEX,
                                  IMG_END,
                                  IMG_SPIKE,
                                  IMG_POOP,
                                  IMG_BACKGROUND,
                                  IMG_DECORATION_0,
                                  IMG_DECORATION_1,
                                  IMG_DECORATION_2};

  for (auto& image : images) {
    asset_manager.textures.insert(
        {image,
         LoadTexture(concat(game_config.resource_dir(), image).c_str())});
  }

  asset_manager.fonts.insert(
      {FONT_SMALL,
       LoadFontEx(
           concat(game_config.resource_dir(), FONT_FIRA_BOLD_SRC).c_str(), 12,
           nullptr, 255)});
  asset_manager.fonts.insert(
      {FONT_MEDIUM,
       LoadFontEx(concat(game_config.resource_dir(), FONT_FIRA_SRC).c_str(), 20,
                  nullptr, 255)});
  asset_manager.fonts.insert(
      {FONT_LARGE,
       LoadFontEx(concat(game_config.resource_dir(), FONT_FIRA_SRC).c_str(), 64,
                  nullptr, 255)});
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

#ifdef DEBUG
    DrawFPS(GetScreenWidth() - 96, GetScreenHeight() - 26);
#endif

    auto next_stage = stage->next_stage();
    if (next_stage.has_value()) {
      LOG_INFO("Stage change: %d -> %d", current_stage, next_stage.value());
      current_stage = next_stage.value();
      stages[current_stage]->init();
    }

    EndDrawing();
  }
}
