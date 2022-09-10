#include "asset_manager.h"

#include <cassert>
#include <cstring>
#include <iostream>

#include "raylib.h"

AssetManager::~AssetManager() {
  for (auto& texture_pair : textures) UnloadTexture(texture_pair.second);
  textures.clear();

  for (auto& font_pair : fonts) UnloadFont(font_pair.second);
  fonts.clear();
}

std::vector<Texture2D*> AssetManager::texture_list(
        const char* file_name_format) const {
  std::vector<Texture2D*> out{};

  for (int i{0};; i++) {
    char file_name[256];
    assert(strlen(file_name_format) < 240);

    sprintf(file_name, file_name_format, i);

    if (textures.count(file_name) == 0) break;

    out.emplace_back((Texture*) (&textures.at(file_name)));
  }

  return out;
}

void AssetManager::preload_textures(std::string folder) {
  FilePathList files = LoadDirectoryFiles(folder.c_str());

  for (int i = 0; i < (int) files.count; i++) {
    textures.insert(
            {GetFileName(files.paths[i]),
             LoadTexture(files.paths[i])});
  }
}

AssetManager asset_manager = AssetManager();
