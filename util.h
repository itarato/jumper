#pragma once

#include <stdarg.h>

#include <cstdio>
#include <string>

#include "raylib.h"

#define BLOCK_SIZE 32
#define WINDOW_BLOCK_WIDTH 40
#define WINDOW_BLOCK_HEIGHT 20

#define PROXIMITY_TRESHOLD 2.0f
#define GRAVITY_ACC 1.15f
#define PARACHUTE_V 1.4f
#define VELOCITY_ZERO_THRESHOLD 1.0f
#define JUMPER_HMOVE_V 5.0f
#define JUMP_FORCE 22.0f
#define FRICTION 0.9f

#define WINDOW_WITDH (WINDOW_BLOCK_WIDTH * BLOCK_SIZE)
#define WINDOW_HEIGH (WINDOW_BLOCK_HEIGHT * BLOCK_SIZE)

#define LOG_INFO(...) log(LOG_LEVEL_INFO, __VA_ARGS__)
#define LOG_WARN(...) log(LOG_LEVEL_WARNING, __VA_ARGS__)
#define LOG_ERR(...) log(LOG_LEVEL_ERROR, __VA_ARGS__)

#define IMG_GROUND "assets/images/ground.png"
#define IMG_BACKGROUND "assets/images/background.png"
#define IMG_ENEMY "assets/images/enemy.png"
#define IMG_COIN "assets/images/coin.png"
#define IMG_LADYBUG_STAND "assets/images/ladybug_stand.png"
#define IMG_LADYBUG_MOVE_0 "assets/images/ladybug_move_0.png"
#define IMG_LADYBUG_MOVE_1 "assets/images/ladybug_move_1.png"
#define IMG_LADYBUG_MOVE_2 "assets/images/ladybug_move_2.png"
#define IMG_LADYBUG_MOVE_3 "assets/images/ladybug_move_3.png"

typedef enum {
  LOG_LEVEL_INFO = 0,
  LOG_LEVEL_WARNING = 1,
  LOG_LEVEL_ERROR = 2,
} LogLevelT;

void log(LogLevelT level, std::string msg, ...);
bool in_range(int number, int min, int max);
int rand_range(int min, int max);
