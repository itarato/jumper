#include "asset_manager.h"

#include <cassert>
#include <cstring>
#include <iostream>

AssetManager::~AssetManager() {
  for (auto& texture_pair : textures) UnloadTexture(texture_pair.second);
  textures.clear();

  for (auto& font_pair : fonts) UnloadFont(font_pair.second);
  fonts.clear();
}

std::vector<Texture2D*> AssetManager::texture_list(
    const char* file_name_format) {
  std::vector<Texture2D*> out{};

  for (int i{0};; i++) {
    char file_name[256];
    assert(strlen(file_name_format) < 240);

    sprintf(file_name, file_name_format, i);

    if (textures.count(file_name) == 0) break;

    out.emplace_back(&textures[file_name]);
  }

  return out;
}

void AssetManager::preload_textures(std::string folder) {
  int file_count;
  char** files = GetDirectoryFiles(folder.c_str(), &file_count);

  for (int i = 0; i < file_count; i++) {
    if (!IsFileExtension(files[i], ".png")) continue;

    textures.insert(
        {files[i],
         LoadTexture(concat(folder.c_str(), files[i], CONCAT_END).c_str())});
  }

  ClearDirectoryFiles();
}

AssetManager asset_manager = AssetManager();
