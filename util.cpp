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

bool in_range(int number, int min, int max) {
  return number >= min && number <= max;
}

void draw_text_align_center(const char* text, int size, Color color) {
  int width = MeasureText(text, size);
  DrawText(text, (GetScreenWidth() - width) / 2, (GetScreenHeight() - size) / 2,
           size, color);
}

Rectangle text_frame(const char* text, int size, int x, int y) {
  return Rectangle{
      (float)x,
      (float)y,
      (float)MeasureText(text, size),
      (float)size,
  };
}
