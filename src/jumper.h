#pragma once

#include <regex>
#include <string>

#include "map.h"
#include "raylib.h"
#include "shared/types.h"
#include "sprite.h"

enum class JumperState {
  Normal = 0,
  Dying = 1,
  Dead = 2,
};

struct DoubleJump {
  int air_jumps;

  DoubleJump() : air_jumps(0) {}

  bool can_jump(MapObjectVerticalState vertical_state) {
    if (vertical_state == MAP_OBJECT_VERTICAL_STATE_ON_FLOOR) {
      air_jumps = 0;
      return true;
    }

    if (air_jumps >= 1) {
      return false;
    }

    air_jumps++;
    return true;
  }

  void reset() { air_jumps = 0; }
};

// Idea: we can add a delayer to it, so after a poop it cannot go immediately.
//       With: .can_powerup():bool and maybe .next_poop_wait():float (for animation)
struct Pooper {
  int progress{0};
  const int upto{42};

  void powerup() {
    progress++;
  }

  void reset() {
    progress = 0;
  }

  bool is_ready() {
    return progress >= upto;
  }
};

enum class JumperEvent {
  CaptureRegex,
  Jump,
  Dash,
  Die,
  OpenDoor,
  FailedDoor,
  Poop,
};

struct JumperEventData {
  Rectangle *frame;
  Rectangle subject;

  explicit JumperEventData(Rectangle *frame) : frame(frame),
                                               subject(*frame) {}
  explicit JumperEventData(Rectangle *frame, Rectangle subject) : frame(frame),
                                                                  subject(subject) {}
};

struct JumperObserver {
  virtual void on_jumper_update(JumperEvent event, JumperEventData data) = 0;
};

struct JumperSubject {
  std::vector<JumperObserver *> observers;

  void subscribe(JumperObserver *observer) {
    observers.push_back(observer);
  }

  void notify_all(JumperEvent event, JumperEventData data) {
    for (auto &observer : observers) {
      observer->on_jumper_update(event, data);
    }
  }
};

struct Jumper : IMapStateUpdatable, JumperSubject {
  Rectangle frame;
  Vector2 v;
  MapObjectState map_state;
  bool is_facing_right;
  Sprite move_sprite;
  Sprite stand_sprite;
  Sprite fly_sprite;
  DoubleJump double_jump;
  std::string regex_raw{};
  JumperState state;
  float dying_rot;
  float dying_fade;
  float dying_scale;
  Pooper pooper{};

  void draw(IntVector2D scroll_offset);
  void update(Map *map);
  void init(Vector2 start_pos);

  [[nodiscard]] Rectangle get_frame() const override;
  [[nodiscard]] Vector2 get_v() const override;
  void set_map_state(MapObjectState mos) override;
  [[nodiscard]] std::regex get_regex() const;

  [[nodiscard]] bool is_dead() const;
  [[nodiscard]] bool is_alive() const;
  void kill();

  Jumper();
};
