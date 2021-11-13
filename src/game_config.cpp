#include "game_config.h"

#include <string>

#include "shared/util.h"

int GameConfig::window_width() {
  return arg_val_or_default("width", 960, convert_string_to_int);
}

int GameConfig::window_height() {
  return arg_val_or_default("height", 640, convert_string_to_int);
}

template<class Out>
Out GameConfig::arg_val_or_default(const char* key, Out def, Out (*conv)(std::string)) {
  auto it = argmap.find(key);
  if (it != argmap.end()) return conv(it->second);

  return def;
}
