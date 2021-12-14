#include "asset_manager.h"

#include <cassert>
#include <cstring>
#include <filesystem>
#include <iostream>

AssetManager::~AssetManager() {
  for (auto& texture : textures)
    UnloadTexture(texture.second);
  textures.clear();

  for (auto& font : fonts)
    UnloadFont(font.second);
  fonts.clear();
}

std::vector<Texture2D*> AssetManager::texture_list(
    const char* file_name_format) {
  std::vector<Texture2D*> out{};

  for (int i{0};; i++) {
    char file_name[256];
    assert(strlen(file_name_format) < 240);

    sprintf(file_name, file_name_format, i);

    if (textures.count(file_name) == 0)
      break;

    out.emplace_back(&textures[file_name]);
  }

  return out;
}

void AssetManager::preload_textures(std::string folder) {
  for (auto const& file_path : std::filesystem::directory_iterator{folder}) {
    if (file_path.path().extension() != ".png")
      continue;

    textures.insert({file_path.path().filename(),
                     LoadTexture(file_path.path().string().c_str())});
  }
}

AssetManager asset_manager = AssetManager();
