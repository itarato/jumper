#include "game_config.h"

#include <string>

#include "defines.h"
#include "shared/util.h"

GameConfig::GameConfig(std::map<std::string, std::string>&& argmap) {
  window_width =
      arg_val_or_default(argmap, "width", 960, convert_string_to_int);
  window_height =
      arg_val_or_default(argmap, "height", 640, convert_string_to_int);

  is_fullscreen = argmap.count("fullscreen") > 0;

  resource_dir = argmap[ASSETS_PATH_KEY];

  auto theme_it = argmap.find("theme");
  image_theme = (theme_it != argmap.end()) ? theme_it->second : "default";

  background_color =
      arg_val_or_default(argmap, "background_color", (unsigned long)0xFAFAFAFF,
                         convert_string_to_ulong);

  is_background_image = get_bool_setting(argmap, "background_image", true);

  auto tile_type =
      arg_val_or_default(argmap, "background_tile", std::string{"full"}, id);
  is_background_horizontal_tile = tile_type == "horizontal";
  is_background_full_tile = tile_type == "full";

  auto map_it = argmap.find("map");
  if (map_it != argmap.end())
    selected_map = std::optional<std::string>{map_it->second};
}

template <class Out>
Out GameConfig::arg_val_or_default(std::map<std::string, std::string>& argmap,
                                   const char* key, Out def,
                                   Out (*conv)(std::string)) {
  auto it = argmap.find(key);
  if (it != argmap.end()) return conv(it->second);

  return def;
}

bool GameConfig::get_bool_setting(std::map<std::string, std::string>& argmap,
                                  const char* name, bool fallback) const {
  if (argmap.count(name) == 0) return fallback;
  return argmap[name] == "1" || argmap[name] == "true";
}
