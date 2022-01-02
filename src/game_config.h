#pragma once

#include <map>
#include <optional>
#include <string>

struct GameConfig {
  explicit GameConfig(std::map<std::string, std::string>&& argmap);

  int window_width;
  int window_height;
  bool is_fullscreen{false};
  std::string resource_dir;
  std::string image_theme;
  unsigned long background_color;
  bool is_background_image{false};
  bool is_background_horizontal_tile{false};
  bool is_background_full_tile{false};
  std::optional<std::string> selected_map{std::nullopt};

 private:
  template <class Out>
  Out arg_val_or_default(std::map<std::string, std::string>& argmap,
                         const char* key, Out def, Out (*conv)(std::string));

  bool get_bool_setting(std::map<std::string, std::string>& argmap,
                        const char* name, bool fallback) const;
};
