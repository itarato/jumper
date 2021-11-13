#include "util.h"

#include <algorithm>
#include <cstdlib>

#include "shared_defines.h"

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

int convert_string_to_int(std::string s) {
  return std::stoi(s);
}

bool in_range(int number, int min, int max) {
  return number >= min && number <= max;
}

float randf() {
  return (float) (rand() % 1000000) / 1000000.0f;
}

int rand_range(int min, int max) { return (rand() % (max - min + 1)) + min; }

Rectangle rec_plus_vector2(Rectangle rec, Vector2 v) {
  return Rectangle{
          rec.x + v.x,
          rec.y + v.y,
          rec.width,
          rec.height,
  };
}

std::vector<std::string> split(std::string word, char delim) {
  size_t prev_delim_pos = -1;
  std::vector<std::string> out{};

  for (size_t i = 0; i < word.size(); i++) {
    if (word.at(i) == delim) {
      out.push_back(word.substr(prev_delim_pos + 1, i - prev_delim_pos - 1));
      prev_delim_pos = i;
    }
  }
  out.push_back(word.substr(prev_delim_pos + 1, word.size() - prev_delim_pos - 1));

  return out;
}

std::vector<IntVector2D> corner_block_coords(Rectangle frame) {
  std::vector<IntVector2D> out{
          IntVector2D{(int) frame.x / BLOCK_SIZE, (int) frame.y / BLOCK_SIZE},                                 // Top left
          IntVector2D{(int) (frame.x + frame.width) / BLOCK_SIZE, (int) frame.y / BLOCK_SIZE},                 // Top right
          IntVector2D{(int) frame.x / BLOCK_SIZE, (int) (frame.y + frame.height) / BLOCK_SIZE},                // Bottom left
          IntVector2D{(int) (frame.x + frame.width) / BLOCK_SIZE, (int) (frame.y + frame.height) / BLOCK_SIZE},// Bottom right
  };

  std::sort(out.begin(), out.end());
  auto last = std::unique(out.begin(), out.end());
  out.erase(last, out.end());

  return out;
}

TileMeta::TileMeta(std::string raw) {
  std::vector<std::string> parts{split(std::move(raw), ',')};
  x = std::stoi(parts[0]);
  y = std::stoi(parts[1]);
  value.swap(parts[2]);
}

void merge_pattern(std::string& base, std::string new_part) {
  if (new_part.empty()) {
    LOG_WARN("Empty new pattern");
    return;
  }

  if (new_part == "[CLR]") {// Clear regex.
    base.clear();
  } else if (new_part == "[REV]") {// Reverse regex.
    std::string new_base{base};
    base.clear();
    std::copy(new_base.rbegin(), new_base.rend(), std::back_inserter(base));
  } else if (new_part.front() == '/' && new_part.back() == '/') {// Replace.
    new_part.pop_back();
    new_part.erase(0, 1);
    base = new_part;
  } else if (new_part.front() == '/') {// To beginning.
    new_part.erase(0, 1);
    base.insert(0, new_part);
  } else if (new_part.back() == '/') {// To end.
    new_part.pop_back();
    base.append(new_part);
  }
}

/**
 * Expected argument format:
 * ./binary -value_argument value flagargument ...
 *
 * @param argc
 * @param argv
 * @return
 */
std::map<std::string, std::string> parse_args(int argc, char** argv) {
  std::map<std::string, std::string> out{};

  std::string prepare_key;
  bool key_is_prepared = false;
  for (int i = 1; i < argc; i++) {
    if (key_is_prepared) {
      std::string val{argv[i]};
      out.insert({prepare_key, val});
      key_is_prepared = false;
    } else if (argv[i][0] == '-') {
      prepare_key = argv[i];
      prepare_key.erase(0, 1);
      key_is_prepared = true;
    } else {
      out.insert({argv[i], ""});
    }
  }

  return out;
}
