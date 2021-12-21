#include "app.h"

#include <string>

#include "asset_manager.h"
#include "defines.h"
#include "raylib.h"
#include "stage_game.h"
#include "stage_menu.h"

App::App(GameConfig&& game_config) : game_config(std::move(game_config)) {}

App::~App() {
  for (auto& stage_pair : stages) {
    delete stage_pair.second;
    stage_pair.second = nullptr;
  }
}

void App::init() {
  bool is_fullscreen = game_config.is_fullscreen;
  InitWindow(is_fullscreen ? 0 : game_config.window_width,
             is_fullscreen ? 0 : game_config.window_height, "Jumper");
  SetTargetFPS(60);

  asset_manager.preload_textures(
      concat(game_config.resource_dir.c_str(), "/images/",
             game_config.image_theme.c_str(), "/", CONCAT_END));

  asset_manager.fonts.insert(
      {FONT_SMALL, LoadFontEx(concat(game_config.resource_dir.c_str(),
                                     FONT_FIRA_BOLD_SRC, CONCAT_END)
                                  .c_str(),
                              12, nullptr, 255)});
  asset_manager.fonts.insert(
      {FONT_MEDIUM, LoadFontEx(concat(game_config.resource_dir.c_str(),
                                      FONT_FIRA_BOLD_SRC, CONCAT_END)
                                   .c_str(),
                               20, nullptr, 255)});
  asset_manager.fonts.insert(
      {FONT_LARGE, LoadFontEx(concat(game_config.resource_dir.c_str(),
                                     FONT_FIRA_BOLD_SRC, CONCAT_END)
                                  .c_str(),
                              64, nullptr, 255)});

  stages.insert({STAGE_MENU, new StageMenu(&game_config)});
  stages.insert({STAGE_GAME, new StageGame(&game_config)});
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
