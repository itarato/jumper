#pragma once

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "asset_manager.h"
#include "defines.h"
#include "raylib.h"
#include "shared/shared_map_schema.h"
#include "shared/types.h"
#include "shared/util.h"

struct ITextureProvider {
  virtual Texture2D* texture() = 0;
  virtual void disable() = 0;
  virtual void enable() = 0;
  [[nodiscard]] virtual Color color() const { return WHITE; }
};

struct NullTextureProvider : ITextureProvider {
  Texture2D* texture() override { return nullptr; }
  void disable() override {}
  void enable() override {}
};

struct SingleTextureProvider : ITextureProvider {
  Texture2D* texture_enabled{nullptr};
  Texture2D* texture_disabled{nullptr};
  bool enabled{true};

  explicit SingleTextureProvider(Texture2D* texture_enabled)
      : ITextureProvider(),
        texture_enabled(texture_enabled),
        texture_disabled(nullptr),
        enabled(true) {}
  SingleTextureProvider(Texture2D* texture_enabled, Texture2D* texture_disabled)
      : ITextureProvider(),
        texture_enabled(texture_enabled),
        texture_disabled(texture_disabled),
        enabled(true) {}

  Texture2D* texture() override;
  void disable() override;
  void enable() override;
  [[nodiscard]] Color color() const override;
};

enum class SpriteTextureProviderState {
  Enabled,
  Disabled,
  Enabling,
  Disabling,
};

struct DisableSpriteTextureProvider : SingleTextureProvider {
  std::vector<Texture2D*> sprite{};
  int step{0};
  int frame_counter{0};
  int step_frame_count{1};
  SpriteTextureProviderState state{SpriteTextureProviderState::Enabled};

  DisableSpriteTextureProvider(Texture2D* texture_enabled,
                               Texture2D* texture_disabled,
                               std::vector<Texture2D*>&& sprite,
                               int step_frame_count)
      : SingleTextureProvider(texture_enabled, texture_disabled),
        sprite(std::move(sprite)),
        step_frame_count(step_frame_count) {}

  Texture2D* texture() override;

  void disable() override;
  void enable() override;
};

struct Tile;

struct TileBehaviour {
  virtual void update(Tile* tile) = 0;
  virtual void draw(const Tile* tile, IntVector2D pos,
                    IntVector2D scroll_offset){};
};

struct Tile {
  TileType type{TILE_AIR};
  std::string pattern{};
  int decoration{-1};
  std::shared_ptr<ITextureProvider> texture_provider{nullptr};
  Rectangle draw_frame{};
  std::shared_ptr<TileBehaviour> behaviour{nullptr};

  explicit Tile(TileType type)
      : type(type),
        draw_frame({0.0f, 0.0f, (float)BLOCK_SIZE, (float)BLOCK_SIZE}) {
    if (type == TILE_GROUND) {
      auto _texture_provider = std::make_shared<SingleTextureProvider>(
          &asset_manager.textures[IMG_GROUND]);

      texture_provider = _texture_provider;
    } else if (type == TILE_END) {
      texture_provider = std::make_shared<SingleTextureProvider>(
          &asset_manager.textures[IMG_END]);
    } else if (type == TILE_DOOR) {
      texture_provider = std::make_shared<DisableSpriteTextureProvider>(
          &asset_manager.textures[IMG_DOOR_CLOSE],
          &asset_manager.textures[IMG_DOOR_OPEN],
          asset_manager.texture_list(IMG_FORMAT_DOOR_CLOSE), 4);
    } else if (type == TILE_REGEX) {
      texture_provider = std::make_shared<SingleTextureProvider>(
          &asset_manager.textures[IMG_REGEX]);
    } else if (type == TILE_TRAP) {
      texture_provider = std::make_shared<SingleTextureProvider>(
          &asset_manager.textures[IMG_SPIKE]);
    } else {
      texture_provider = std::make_shared<NullTextureProvider>();
    }
  }

  void update() {
    if (!behaviour) return;

    behaviour->update(this);
  }

  void draw(IntVector2D pos, IntVector2D scroll_offset) const {
    if (!behaviour) return;

    behaviour->draw(this, pos, scroll_offset);
  }

  [[nodiscard]] bool is_solid() const {
    return is_enabled() && is_tile_type_solid(type);
  }

  bool is_enabled() const { return _is_enabled; }

  void enable() {
    _is_enabled = true;
    texture_provider->enable();
  }

  void disable() {
    _is_enabled = false;
    texture_provider->disable();
  }

  void vertical_flip() { draw_frame.height *= -1; }

  std::string formatted_pattern() {
    std::string out{pattern};

    if (is_regex_prepend() || is_regex_replace() || is_regex_transformation()) {
      out.erase(0, 1);
    }

    if (is_regex_append() || is_regex_replace() || is_regex_transformation()) {
      out.erase(out.size() - 1, 1);
    }

    return out;
  }

  // TODO: Make these memoized vars.
  bool has_decoration() const { return decoration >= 0; }

  bool is_regex() const { return type == TILE_REGEX; }

  bool is_regex_replace() const {
    if (pattern.size() <= 1) return false;
    return pattern[0] == '/' && pattern[pattern.size() - 1] == '/';
  }

  bool is_regex_prepend() const {
    if (pattern.size() <= 1) return false;
    return pattern[0] == '/' && !is_regex_replace();
  }

  bool is_regex_append() const {
    if (pattern.size() <= 1) return false;
    return pattern[pattern.size() - 1] == '/' && !is_regex_replace();
  }

  bool is_regex_transformation() const {
    return pattern == "[CLR]" || pattern == "[REV]";
  }

 private:
  bool _is_enabled{true};
};

struct TimedDoorBehaviour : TileBehaviour {
  const int timeout{0};
  bool is_timing{false};
  int counter{0};

  TimedDoorBehaviour(int timeout) : timeout(timeout) {}

  void update(Tile* tile) override {
    if (!is_timing) {
      if (!tile->is_enabled()) {
        is_timing = true;
        counter = timeout;
      }
    }

    if (is_timing) {
      counter--;

      if (counter <= 0) {
        is_timing = false;
        tile->enable();
      }
    }
  }

  void draw(const Tile* tile, IntVector2D pos,
            IntVector2D scroll_offset) override {
    if (is_timing) {
      DrawRectangle((pos.x * BLOCK_SIZE) - scroll_offset.x,
                    (pos.y * BLOCK_SIZE) - scroll_offset.y, BLOCK_SIZE, 12,
                    DARKBLUE);
      DrawText(std::to_string(counter).c_str(),
               (pos.x * BLOCK_SIZE) - scroll_offset.x + 2,
               (pos.y * BLOCK_SIZE) - scroll_offset.y + 1, 10, WHITE);
    }
  }
};

struct Map {
  std::vector<std::vector<Tile>> map{};
  size_t block_width{0};
  size_t block_height{0};
  Vector2 start_pos{};
  Vector2 end_pos{};

  void build(const std::string& map_file_path);
  void update();
  void draw(IntVector2D scroll_offset);

  std::optional<int> next_floor(Rectangle p);
  std::optional<int> next_ceiling(Rectangle p);
  std::optional<int> next_left(Rectangle p);
  std::optional<int> next_right(Rectangle p);

  void load_map(const std::string& file_path);
  [[nodiscard]] bool is_solid_tile(IntVector2D coord) const;
  [[nodiscard]] Tile& get_tile(IntVector2D coord);

  std::vector<IntVector2D> coords_of_tile_type(TileType type);

  [[nodiscard]] bool is_inside_map(IntVector2D coord) const;

  [[nodiscard]] size_t pixel_height() const;
  [[nodiscard]] size_t pixel_width() const;
};
