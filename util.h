#pragma once

#include <cstdio>
#include <stdarg.h>
#include <string>

#define LOG_INFO(...) log(LOG_LEVEL_INFO, __VA_ARGS__)
#define LOG_WARN(...) log(LOG_LEVEL_WARNING, __VA_ARGS__)
#define LOG_ERR(...) log(LOG_LEVEL_ERROR, __VA_ARGS__)

typedef enum {
  LOG_LEVEL_INFO = 0,
  LOG_LEVEL_WARNING = 1,
  LOG_LEVEL_ERROR = 2,
} LogLevelT;

void log(LogLevelT level, std::string msg, ...) {
  va_list argptr;
  va_start(argptr, msg);

  std::string log_level;
  switch (level) {
  case LOG_LEVEL_INFO:
    log_level = "INFO";
    break;

  case LOG_LEVEL_WARNING:
    log_level = "WARNING";
    break;

  case LOG_LEVEL_ERROR:
    log_level = "ERROR";
    break;

  default:
    log_level = "-";
    break;
  }

  printf("[JUMPER::%s] ", log_level.c_str());
  vprintf(msg.c_str(), argptr);
  printf("\n");
}
