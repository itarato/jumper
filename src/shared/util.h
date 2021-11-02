#pragma once

#include <stdarg.h>

#include <cstdio>
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

void log(LogLevelT level, std::string msg, ...);
bool in_range(int number, int min, int max);
int rand_range(int min, int max);

Rectangle rec_plus_vector2(Rectangle rec, Vector2 v);

std::vector<std::string> split(std::string word, char delim);

[[nodiscard]] std::vector<IntVector2D> corner_block_coords(Rectangle frame);

struct TileMeta {
  int x;
  int y;
  std::string value;

  explicit TileMeta(std::string raw);
};

void merge_pattern(std::string &base, std::string new_part);
