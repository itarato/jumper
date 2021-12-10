#pragma once

#include <map>
#include <optional>
#include <string>

struct GameConfig {
  std::map<std::string, std::string> argmap;

  explicit GameConfig(std::map<std::string, std::string>&& argmap)
      : argmap(std::move(argmap)) {}

  int window_width();
  int window_height();
  bool is_fullscreen();
  std::string resource_dir();
  std::string image_theme();

  std::optional<std::string> selected_map();

 private:
  template <class Out>
  Out arg_val_or_default(const char* key, Out def, Out (*conv)(std::string));
};
