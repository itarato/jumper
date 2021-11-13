#pragma once

#include <map>
#include <streambuf>
#include <string>

#include "raylib.h"
#include "shared/util.h"

struct AssetManager {
  std::map<std::string, Texture2D> textures;
  std::map<std::string, Font> fonts;
  ~AssetManager();
};

extern AssetManager asset_manager;