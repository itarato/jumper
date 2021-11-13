#pragma once

#include <map>
#include <string>

struct GameConfig {
  std::map<std::string, std::string> argmap;

  GameConfig(std::map<std::string, std::string> argmap) : argmap(argmap) {}

  int window_width();
  int window_height();

  private:
  template<class Out>
  Out arg_val_or_default(const char* key, Out def, Out (*conv)(std::string));
};
