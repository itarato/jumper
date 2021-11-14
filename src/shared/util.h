#pragma once

#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "raylib.h"
#include "types.h"

#define LOG_INFO(...) log(LOG_LEVEL_INFO, __VA_ARGS__)
#define LOG_WARN(...) log(LOG_LEVEL_WARNING, __VA_ARGS__)
#define LOG_ERR(...) log(LOG_LEVEL_ERROR, __VA_ARGS__)

typedef enum {
  LOG_LEVEL_INFO = 0,
  LOG_LEVEL_WARNING = 1,
  LOG_LEVEL_ERROR = 2,
} LogLevelT;

int convert_string_to_int(std::string s);

void log(LogLevelT level, std::string msg, ...);
bool in_range(int number, int min, int max);
int rand_range(int min, int max);
float randf();

Rectangle rec_plus_vector2(Rectangle rec, Vector2 v);
Rectangle shrink(Rectangle rec, float n);
Vector2 midpoint(Rectangle rec);
Vector2 position_of(Rectangle rec);

std::vector<std::string> split(std::string word, char delim);

[[nodiscard]] std::vector<IntVector2D> corner_block_coords(Rectangle frame);

struct TileMeta {
  int x;
  int y;
  std::string value;

  explicit TileMeta(std::string raw);
};

void merge_pattern(std::string& base, std::string new_part);

struct Timer {
  std::chrono::time_point<std::chrono::system_clock> _tick{};

  Timer() = default;

  void tick() {
    _tick = std::chrono::high_resolution_clock::now();
  }

  [[nodiscard]] double tock() const {
    auto old_tick = _tick;
    auto new_tick = std::chrono::high_resolution_clock::now();

    return std::chrono::duration<double, std::milli>(new_tick - old_tick).count();
  }

  void tock_and_dump(std::string msg = "") const {
    std::cout << msg << ": " << std::fixed << std::setprecision(4) << tock() << "ms" << std::endl;
  }
};

struct Ticker {
  uint64_t _tick{0};

  void tick() { _tick++; }

  void dump(std::string msg = "") const {
    std::cout << msg << ": " << _tick << " ticks" << std::endl;
  }
};

std::map<std::string, std::string> parse_args(int argc, char** argv);