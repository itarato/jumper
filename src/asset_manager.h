#pragma once

#include <map>
#include <streambuf>
#include <string>
#include <vector>

#include "raylib.h"
#include "shared/util.h"

struct AssetManager {
  std::map<std::string, Texture2D> textures;
  std::map<std::string, Font> fonts;
  std::vector<Texture2D*> texture_list(const char* file_name_format);
  void preload_textures(std::string folder);

  ~AssetManager();
};

extern AssetManager asset_manager;
