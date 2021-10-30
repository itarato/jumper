#include "util.h"
#include "defines.h"

#include <cstdlib>

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

IntVector2D top_left_block_coord(Rectangle rectangle) {
  return IntVector2D{(int)rectangle.x / BLOCK_SIZE, (int)rectangle.y / BLOCK_SIZE};
}

IntVector2D top_right_block_coord(Rectangle rectangle) {
  return IntVector2D{(int)(rectangle.x + rectangle.width) / BLOCK_SIZE, (int)rectangle.y / BLOCK_SIZE};
}

IntVector2D bottom_left_block_coord(Rectangle rectangle) {
  return IntVector2D{(int)rectangle.x / BLOCK_SIZE, (int)(rectangle.y + rectangle.height) / BLOCK_SIZE};
}

IntVector2D bottom_right_block_coord(Rectangle rectangle) {
  return IntVector2D{(int)(rectangle.x + rectangle.width) / BLOCK_SIZE, (int)(rectangle.y + rectangle.height) / BLOCK_SIZE};
}

Rectangle rec_plus_vector2(Rectangle rec, Vector2 v) {
  return Rectangle{
    rec.x + v.x,
    rec.y + v.y,
    rec.width,
    rec.height,
  };
}

std::vector<std::string> split(std::string word, char delim) {
  int prev_delim_pos = -1;
  std::vector<std::string> out{};

  for (int i = 0; i < word.size(); i++) {
    if (word.at(i) == delim) {
      out.push_back(word.substr(prev_delim_pos + 1, i - prev_delim_pos - 1));
      prev_delim_pos = i;
    }
  }
  out.push_back(word.substr(prev_delim_pos + 1, word.size() - prev_delim_pos - 1));

  return out;
}
