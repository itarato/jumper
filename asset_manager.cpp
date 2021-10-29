#include "asset_manager.h"

#include <iostream>
#include <ranges>

AssetManager::~AssetManager() {
  for (auto & texture : textures | std::views::values) {
    UnloadTexture(texture);
  }
  textures.clear();
}

AssetManager asset_manager = AssetManager();
