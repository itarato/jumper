#pragma once

#include <map>
#include <string>

#include "raylib.h"
#include "util.h"

struct AssetManager {
  std::map<std::string, Texture2D> textures;

  AssetManager() { LOG_INFO("AMCTOR"); }
};

extern AssetManager asset_manager;