#include "util.h"

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

bool is_bit_on(unsigned int number, int index) {
  return ((number >> index) & 1) == 1;
}

Vector2 offset_x(Vector2 v, float offset) {
  v.x += offset;
  return v;
}

Vector2 offset_y(Vector2 v, float offset) {
  v.y += offset;
  return v;
}
