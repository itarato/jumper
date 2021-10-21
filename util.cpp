#include "util.h"

#include <stdlib.h>

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

bool in_range(int number, int min, int max) {
  return number >= min && number <= max;
}

int rand_range(int min, int max) { return (rand() % (max - min + 1)) + min; }
