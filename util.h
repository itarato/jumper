#pragma once

#include "raylib.h"
#include <cstdio>
#include <stdarg.h>
#include <string>

#define BLOCK_SIZE 32
#define WINDOW_BLOCK_WIDTH 40
#define WINDOW_BLOCK_HEIGHT 20

#define PROXIMITY_TRESHOLD 2.0f
#define GRAVITY_ACC 1.15f
#define VELOCITY_ZERO_THRESHOLD 1.0f
#define JUMPER_HMOVE_V 5.0f
#define JUMP_FORCE 22.0f
#define FRICTION 0.8f

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

bool is_bit_on(unsigned int number, int index);
