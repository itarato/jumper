#include "asset_manager.h"

#include <cassert>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <ranges>

AssetManager::~AssetManager() {
  for (auto& texture : textures | std::views::values)
    UnloadTexture(texture);
  textures.clear();

  for (auto& font : fonts | std::views::values)
    UnloadFont(font);
  fonts.clear();
}

std::vector<Texture2D*> AssetManager::texture_list(
    const char* file_name_format) {
  std::vector<Texture2D*> out{};

  for (int i{0};; i++) {
    char file_name[256];
    assert(strlen(file_name_format) < 240);

    sprintf(file_name, file_name_format, i);

    if (!textures.contains(file_name)) {
      break;
    }

    out.emplace_back(&textures[file_name]);
  }

  return out;
}

AssetManager asset_manager = AssetManager();
