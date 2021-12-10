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
  return argmap.contains("fullscreen");
}

std::string GameConfig::resource_dir() {
  return argmap[ASSETS_PATH_KEY];
}

std::string GameConfig::image_theme() {
  return "default";
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
