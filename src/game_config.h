#pragma once

#include <map>
#include <optional>
#include <string>

// Fixme: setup instance vars instead of a map lookup in each iteration.

struct GameConfig {
  std::map<std::string, std::string> argmap;

  explicit GameConfig(std::map<std::string, std::string>&& argmap)
      : argmap(std::move(argmap)) {}

  int window_width();
  int window_height();
  bool is_fullscreen();
  std::string resource_dir();
  std::string image_theme();
  unsigned long background_color();
  bool is_background_image();
  bool is_background_horizontal_tile();
  bool is_background_full_tile();

  std::optional<std::string> selected_map();

 private:
  template <class Out>
  Out arg_val_or_default(const char* key, Out def, Out (*conv)(std::string));
};
