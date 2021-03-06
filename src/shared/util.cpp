#include "util.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>

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

int convert_string_to_int(std::string s) { return std::stoi(s); }

unsigned long convert_string_to_ulong(std::string s) { return std::stoul(s); }

std::string id(std::string s) { return s; }

bool in_range(int number, int min, int max) {
  return number >= min && number <= max;
}

int rand_range(int min, int max) { return (rand() % (max - min + 1)) + min; }

float randf() { return (float)(rand() & 0xFFFF) / 0xFFFF; }

float randf(float min, float max) { return randf() * (max - min) + min; }

Rectangle rec_plus_vector2(Rectangle rec, Vector2 v) {
  return Rectangle{
      rec.x + v.x,
      rec.y + v.y,
      rec.width,
      rec.height,
  };
}

Rectangle shrink(Rectangle rec, float n) {
  return Rectangle{
      rec.x + n,
      rec.y + n,
      rec.width - n * 2.0f,
      rec.height - n * 2.0f,
  };
}

Vector2 relative_midpoint(Rectangle rec) {
  return Vector2{
      rec.width / 2.0f,
      rec.height / 2.0f,
  };
}

Vector2 absolute_midpoint(Rectangle rec) {
  return Vector2{
      rec.x + rec.width / 2.0f,
      rec.y + rec.height / 2.0f,
  };
}

Vector2 absolute_midpoint(Rectangle lhs, Rectangle rhs) {
  Vector2 lhs_mid{absolute_midpoint(lhs)};
  Vector2 rhs_mid{absolute_midpoint(rhs)};
  return absolute_midpoint(lhs_mid, rhs_mid);
}

Vector2 absolute_midpoint(Vector2 lhs, Vector2 rhs) {
  return Vector2{(lhs.x + rhs.x) / 2.0f, (lhs.y + rhs.y) / 2.0f};
}

Vector2 position_of(Rectangle rec) { return Vector2{rec.x, rec.y}; }

void mut_sum(Vector2& base, Vector2 offs) {
  base.x += offs.x;
  base.y += offs.y;
}

void mut_mul(Vector2& base, float m) {
  base.x *= m;
  base.y *= m;
}

Vector2 mul(Vector2 base, float m) { return Vector2{base.x * m, base.y * m}; }

float gravity_pull(float vy, float acc) {
  if (fabsf(vy) < 0.1f) return 1.0f;
  if (vy <= 0.0f) return vy * acc;
  return vy / acc;
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
  out.push_back(
      word.substr(prev_delim_pos + 1, word.size() - prev_delim_pos - 1));

  return out;
}

std::vector<IntVector2D> corner_block_coords(Rectangle frame) {
  std::vector<IntVector2D> out{
      IntVector2D{(int)frame.x / BLOCK_SIZE,
                  (int)frame.y / BLOCK_SIZE},  // Top left
      IntVector2D{(int)(frame.x + frame.width) / BLOCK_SIZE,
                  (int)frame.y / BLOCK_SIZE},  // Top right
      IntVector2D{(int)frame.x / BLOCK_SIZE,
                  (int)(frame.y + frame.height) / BLOCK_SIZE},  // Bottom left
      IntVector2D{(int)(frame.x + frame.width) / BLOCK_SIZE,
                  (int)(frame.y + frame.height) / BLOCK_SIZE},  // Bottom right
  };

  std::sort(out.begin(), out.end());
  auto last = std::unique(out.begin(), out.end());
  out.erase(last, out.end());

  return out;
}

TileMeta::TileMeta(std::string raw) {
  std::vector<std::string> parts{split(std::move(raw), ',')};

  x = std::stoi(parts[1]);
  y = std::stoi(parts[2]);

  if (parts[0] == "pattern") {
    pattern.swap(parts[3]);
  } else if (parts[0] == "decoration") {
    decoration = std::stoi(parts[3]);
  } else if (parts[0] == "door_timer") {
    door_timeout = std::stoi(parts[3]);
  } else if (parts[0] == "tutorial") {
    tutorial.swap(parts[3]);
  } else {
    PANIC("Cannot read map meta: %s", parts[0].c_str());
  }
}

void merge_pattern(std::string& base, std::string new_part) {
  if (new_part.empty()) {
    LOG_WARN("Empty new pattern");
    return;
  }

  if (new_part == "[CLR]") {  // Clear regex.
    base.clear();
  } else if (new_part == "[REV]") {  // Reverse regex.
    std::string new_base{base};
    base.clear();
    std::copy(new_base.rbegin(), new_base.rend(), std::back_inserter(base));
  } else if (new_part.front() == '/' && new_part.back() == '/') {  // Replace.
    new_part.pop_back();
    new_part.erase(0, 1);
    base = new_part;
  } else if (new_part.front() == '/') {  // To beginning.
    new_part.erase(0, 1);
    base.insert(0, new_part);
  } else if (new_part.back() == '/') {  // To end.
    new_part.pop_back();
    base.append(new_part);
  } else {  // Replace too.
    base = new_part;
  }
}

/**
 * Expected argument format:
 * ./binary -value_argument value
 * or
 * ./binary conf_file
 */
std::map<std::string, std::string> parse_args(int argc, char** argv) {
  std::map<std::string, std::string> out{};

  std::string prepare_key;
  bool key_is_prepared = false;
  for (int i = 1; i < argc; i++) {
    if (key_is_prepared) {
      std::string val{argv[i]};

      if (prepare_key == "conf") {
        auto configuration = read_conf(val);
        for (auto [k, v] : configuration) {
          out.insert_or_assign(k, v);
        }
      } else {
        out.insert({prepare_key, val});
      }
      key_is_prepared = false;
    } else if (argv[i][0] == '-') {
      prepare_key = argv[i];
      prepare_key.erase(0, 1);
      key_is_prepared = true;
    } else {
      PANIC("Unsupported argument format");
    }
  }

  std::string exec_dir{GetDirectoryPath(argv[0])};
#if __APPLE__
  exec_dir.append("/..");
  exec_dir.append("/Resources");
  exec_dir.append("/assets");
#else
  exec_dir.append("/assets");
#endif

  out.insert({"assets_dir", exec_dir});

  return out;
}

void SimpleTimer::start() {
  start_time = GetTime();
  stopped = false;
}

void SimpleTimer::stop() {
  if (stopped) return;

  end_time = GetTime();
  stopped = true;
}

int SimpleTimer::minutes() const {
  double finish_time{stopped ? end_time : GetTime()};
  return (int)(finish_time - start_time) / 60;
}

int SimpleTimer::seconds() const {
  double finish_time{stopped ? end_time : GetTime()};
  return (int)(finish_time - start_time) % 60;
}

std::string concat(const char* s, ...) {
  std::string out{s};

  va_list args;
  va_start(args, s);

  for (;;) {
    std::string next{va_arg(args, const char*)};
    if (next.empty()) break;

    out.append(next);
  }
  va_end(args);

  return out;
}

std::map<std::string, std::string> read_conf(std::string file_name) {
  std::map<std::string, std::string> out{};

  std::ifstream file{file_name};
  if (!file.is_open()) {
    PANIC("Cannot open conf file\n");
  }

  std::string line{};

  while (getline(file, line)) {
    // If comment line or empty - skip.
    if (line[0] == '#' && line.empty()) {
      continue;
    }

    std::vector<std::string> parts{split(std::move(line), ' ')};

    if (parts.size() != 2 && parts[0].empty()) {
      PANIC("Invalid setting in conf file\n");
    }

    out.insert({parts[0], parts[1]});
  }

  file.close();

  return out;
}
