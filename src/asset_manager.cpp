#include "asset_manager.h"

#include <iostream>
#include <ranges>

AssetManager::~AssetManager() {
  for (auto & texture : textures | std::views::values) UnloadTexture(texture);
  textures.clear();

  for (auto & font : fonts | std::views::values) UnloadFont(font);
  fonts.clear();
}

AssetManager asset_manager = AssetManager();
