#include "game_config.h"

#include <string>

#include "defines.h"
#include "shared/util.h"

int GameConfig::window_width() {
  return arg_val_or_default("width", 960, convert_string_to_int);
}

int GameConfig::window_height() {
  return arg_val_or_default("height", 640, convert_string_to_int);
}

bool GameConfig::is_fullscreen() {
  return argmap.count("fullscreen") > 0;
}

std::string GameConfig::resource_dir() {
  return argmap[ASSETS_PATH_KEY];
}

std::string GameConfig::image_theme() {
  auto it = argmap.find("theme");
  if (it != argmap.end())
    return it->second;

  return "default";
}

unsigned long GameConfig::background_color() {
  return arg_val_or_default("background_color", (unsigned long)0xFAFAFAFF,
                            convert_string_to_ulong);
}

bool GameConfig::is_background_image() {
  return argmap.count("background_image") > 0;
}

bool GameConfig::is_background_horizontal_tile() {
  if (argmap.count("background_tile") == 0) {
    return false;
  }
  return argmap["background_tile"] == "horizontal";
}

bool GameConfig::is_background_full_tile() {
  if (argmap.count("background_tile") == 0) {
    return false;
  }
  return argmap["background_tile"] == "full";
}

template <class Out>
Out GameConfig::arg_val_or_default(const char* key,
                                   Out def,
                                   Out (*conv)(std::string)) {
  auto it = argmap.find(key);
  if (it != argmap.end())
    return conv(it->second);

  return def;
}

std::optional<std::string> GameConfig::selected_map() {
  auto it = argmap.find("map");
  if (it != argmap.end())
    return std::optional<std::string>{it->second};

  return std::nullopt;
}
