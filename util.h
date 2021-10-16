#pragma once

#include <cstdio>
#include <stdarg.h>
#include <string>

#define BLOCK_SIZE 32
#define WINDOW_BLOCK_WIDTH 40
#define WINDOW_BLOCK_HEIGHT 20

#define WINDOW_WITDH (WINDOW_BLOCK_WIDTH * BLOCK_SIZE)
#define WINDOW_HEIGH (WINDOW_BLOCK_HEIGHT * BLOCK_SIZE)

#define LOG_INFO(...) log(LOG_LEVEL_INFO, __VA_ARGS__)
#define LOG_WARN(...) log(LOG_LEVEL_WARNING, __VA_ARGS__)
#define LOG_ERR(...) log(LOG_LEVEL_ERROR, __VA_ARGS__)

typedef enum {
  LOG_LEVEL_INFO = 0,
  LOG_LEVEL_WARNING = 1,
  LOG_LEVEL_ERROR = 2,
} LogLevelT;

void log(LogLevelT level, std::string msg, ...);
