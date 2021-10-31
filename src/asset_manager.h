#pragma once

#include <map>
#include <streambuf>
#include <string>

#include "raylib.h"
#include "shared/util.h"

struct AssetManager {
  std::map<std::string, Texture2D> textures;
  ~AssetManager();
};

extern AssetManager asset_manager;