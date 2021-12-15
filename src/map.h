#pragma once

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <optional>
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
  [[nodiscard]] virtual Color color() const { return WHITE; }
};

struct NullTextureProvider : ITextureProvider {
  Texture2D* texture() override { return nullptr; }
  void disable() override {}
};

struct SingleTextureProvider : ITextureProvider {
  Texture2D* texture_enabled;
  Texture2D* texture_disabled;
  bool enabled;
  float _fade{1.0};

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
  [[nodiscard]] Color color() const override;

  void set_fade(float v);
};

struct DisableSpriteTextureProvider : SingleTextureProvider {
  std::vector<Texture2D*> disable_sprite;
  int disable_step{0};
  bool disable_sequence_completed{false};
  int frame_counter{0};
  int step_frame_count;

  DisableSpriteTextureProvider(Texture2D* texture_enabled,
                               Texture2D* texture_disabled,
                               std::vector<Texture2D*>&& disable_sprite,
                               int step_frame_count)
      : SingleTextureProvider(texture_enabled, texture_disabled),
        disable_sprite(std::move(disable_sprite)),
        step_frame_count(step_frame_count) {}

  Texture2D* texture() override;
};

struct Tile {
  TileType type = TILE_AIR;
  std::string pattern{""};
  int decoration{-1};
  bool is_enabled = true;
  std::shared_ptr<ITextureProvider> texture_provider;
  Rectangle draw_frame;

  explicit Tile(TileType type)
      : type(type),
        draw_frame({0.0f, 0.0f, (float)BLOCK_SIZE, (float)BLOCK_SIZE}) {
    if (type == TILE_GROUND) {
      auto _texture_provider = std::make_shared<SingleTextureProvider>(
          &asset_manager.textures[IMG_GROUND]);
      _texture_provider->set_fade(randf(0.8f, 1.0f));

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

  [[nodiscard]] bool is_solid() const {
    return is_enabled && is_tile_type_solid(type);
  }
  void disable() {
    is_enabled = false;
    texture_provider->disable();
  }

  void vertical_flip() { draw_frame.height *= -1; }
};

struct Map {
  std::vector<std::vector<Tile>> map;
  size_t block_width;
  size_t block_height;
  Vector2 start_pos;
  Vector2 end_pos;

  void build(const std::string& map_file_path);
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
