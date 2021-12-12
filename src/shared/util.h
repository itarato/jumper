#pragma once

#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "raylib.h"
#include "types.h"

#define LOG_INFO(...) log(LOG_LEVEL_INFO, __VA_ARGS__)
#define LOG_WARN(...) log(LOG_LEVEL_WARNING, __VA_ARGS__)
#define LOG_ERR(...) log(LOG_LEVEL_ERROR, __VA_ARGS__)

#define CONCAT_END ""

#define PANIC(...)                \
  {                               \
    fprintf(stderr, __VA_ARGS__); \
    exit(EXIT_FAILURE);           \
  }

typedef enum {
  LOG_LEVEL_INFO = 0,
  LOG_LEVEL_WARNING = 1,
  LOG_LEVEL_ERROR = 2,
} LogLevelT;

int convert_string_to_int(std::string s);
unsigned long convert_string_to_ulong(std::string s);

void log(LogLevelT level, std::string msg, ...);
bool in_range(int number, int min, int max);
int rand_range(int min, int max);
float randf();
float randf(float min, float max);

Rectangle rec_plus_vector2(Rectangle rec, Vector2 v);
Rectangle shrink(Rectangle rec, float n);
Vector2 relative_midpoint(Rectangle rec);
Vector2 absolute_midpoint(Rectangle rec);
Vector2 absolute_midpoint(Rectangle lhs, Rectangle rhs);
Vector2 absolute_midpoint(Vector2 lhs, Vector2 rhs);
Vector2 position_of(Rectangle rec);
void mut_sum(Vector2& base, Vector2 offs);
void mut_mul(Vector2& base, float m);
Vector2 mul(Vector2 base, float m);

float gravity_pull(float vy, float acc = 0.7f);

std::vector<std::string> split(std::string word, char delim);

[[nodiscard]] std::vector<IntVector2D> corner_block_coords(Rectangle frame);

struct TileMeta {
  int x;
  int y;
  std::string pattern{""};
  int decoration{-1};

  explicit TileMeta(std::string raw);

  bool has_pattern() { return !pattern.empty(); }
  bool has_decoration() { return decoration >= 0; }
};

void merge_pattern(std::string& base, std::string new_part);

/**
 * High precision timer to measure time spent on execution.
 * Used for debugging.
 */
struct DebugTimer {
  std::chrono::time_point<std::chrono::system_clock> _tick{};

  DebugTimer() = default;

  void tick() { _tick = std::chrono::high_resolution_clock::now(); }

  [[nodiscard]] double tock() const {
    auto old_tick = _tick;
    auto new_tick = std::chrono::high_resolution_clock::now();

    return std::chrono::duration<double, std::milli>(new_tick - old_tick)
        .count();
  }

  void tock_and_dump(std::string msg = "") const {
    std::cout << msg << ": " << std::fixed << std::setprecision(4) << tock()
              << "ms" << std::endl;
  }
};

/**
 * Debugger ticker that can count occurrances of events.
 */
struct Ticker {
  uint64_t _tick{0};

  void tick() { _tick++; }

  void dump(std::string msg = "") const {
    std::cout << msg << ": " << _tick << " ticks" << std::endl;
  }
};

std::map<std::string, std::string> parse_args(int argc, char** argv);

/**
 * Simple minute + second timer.
 */
struct SimpleTimer {
  double start_time{0.0f};
  double end_time{0.0f};
  bool stopped{false};

  SimpleTimer() = default;

  void start();
  void stop();
  [[nodiscard]] int minutes() const;
  [[nodiscard]] int seconds() const;
};

/**
 * Provides a sin-modulated value in a range without an end.
 */
struct Phaser {
  float phase{0.0f};
  float min{0.0f};
  float max{1.0f};
  float speed{0.2f};

  Phaser() = default;
  Phaser(float min, float max, float speed = 0.2f)
      : min(min), max(max), speed(speed) {}

  [[nodiscard]] float value() {
    float out{((sinf(phase) + 1.0f) * (max - min)) / 2.0f + min};
    phase += speed;
    return out;
  }
};

/**
 * A boolean that can be read as true only once - and then it immediately turns
 * to false.
 */
struct OneTimeBool {
  bool b{false};

  void set() { b = true; }

  bool get() {
    bool old{b};
    b = false;
    return old;
  }

  void reset() { b = false; }
};

/**
 * Provides a countdown feature.
 */
struct Countdown {
  const uint32_t frames;
  uint32_t counter{0};

  Countdown(uint32_t frames) : frames(frames) {}

  void reset() { counter = frames; }
  void cancel() { counter = 0; }
  bool is_completed() const { return counter == 0; }
  void tick() {
    if (is_completed())
      return;

    counter--;
  }
  uint32_t get() const { return counter; }
};

/**
 * Must end with empty string ("").
 */
std::string concat(const char* s, ...);

std::map<std::string, std::string> read_conf(std::string file_name);
