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

typedef enum {
  MAP_OBJECT_VERTICAL_STATE_HIT_CEILING = 0,
  MAP_OBJECT_VERTICAL_STATE_REACHING_TOP = 1,
  MAP_OBJECT_VERTICAL_STATE_JUMP = 2,
  MAP_OBJECT_VERTICAL_STATE_ON_FLOOR = 3,
  MAP_OBJECT_VERTICAL_STATE_FALLING = 4,
} MapObjectVerticalState;

struct MapObjectState {
  union {
    int floor;
    int ceiling;
  };
  MapObjectVerticalState type;
};

struct IMapStateUpdatable {
  [[nodiscard]] virtual Rectangle get_frame() const = 0;
  [[nodiscard]] virtual Vector2 get_v() const = 0;
  virtual void set_map_state(MapObjectState mos) = 0;
};

struct ITextureProvider {
  virtual Texture2D* texture() const = 0;
  virtual void disable() = 0;
  virtual Color color() const { return WHITE; }
};

struct NullTextureProvider : ITextureProvider {
  Texture2D* texture() const override {
    return nullptr;
  }
  void disable() override {}
};

struct SingleTextureProvider : ITextureProvider {
  Texture2D* texture_enabled;
  Texture2D* texture_disabled;
  bool enabled;
  float _fade{1.0};

  SingleTextureProvider(Texture2D* texture_enabled) : ITextureProvider(),
                                                      texture_enabled(texture_enabled),
                                                      texture_disabled(nullptr),
                                                      enabled(true) {}
  SingleTextureProvider(Texture2D* texture_enabled, Texture2D* texture_disabled) : ITextureProvider(),
                                                                                   texture_enabled(texture_enabled),
                                                                                   texture_disabled(texture_disabled),
                                                                                   enabled(true) {}

  Texture2D* texture() const override;
  void disable() override;
  Color color() const override;

  void set_fade(float v);
};

struct Tile {
  TileType type = TILE_AIR;
  std::string value{};
  bool is_enabled = true;
  std::shared_ptr<ITextureProvider> texture_provider;

  explicit Tile(TileType type) : type(type) {
    if (type == TILE_GROUND) {
      auto _texture_provider = std::make_shared<SingleTextureProvider>(&asset_manager.textures[IMG_GROUND]);
      _texture_provider->set_fade(randf(0.7f, 1.0f));

      texture_provider = _texture_provider;
    } else if (type == TILE_END) {
      texture_provider = std::make_shared<SingleTextureProvider>(&asset_manager.textures[IMG_END]);
    } else if (type == TILE_DOOR) {
      texture_provider = std::make_shared<SingleTextureProvider>(&asset_manager.textures[IMG_DOOR_CLOSE], &asset_manager.textures[IMG_DOOR_OPEN]);
    } else if (type == TILE_REGEX) {
      texture_provider = std::make_shared<SingleTextureProvider>(&asset_manager.textures[IMG_REGEX]);
    } else if (type == TILE_TRAP) {
      texture_provider = std::make_shared<SingleTextureProvider>(&asset_manager.textures[IMG_SPIKE]);
    } else {
      texture_provider = std::make_shared<NullTextureProvider>();
    }
  }

  [[nodiscard]] bool is_solid() const { return is_enabled && is_tile_type_solid(type); }
  void disable() {
    is_enabled = false;
    texture_provider->disable();
  }
};

struct Map {
  std::vector<std::vector<Tile>> map;
  size_t block_width;
  size_t block_height;
  // FIXME -> we could generally memoize these
  Vector2 start_pos;
  Vector2 end_pos;

  void build(std::string map_file_path);
  void draw(IntVector2D scroll_offset);

  std::optional<int> next_floor(Rectangle p);
  std::optional<int> next_ceiling(Rectangle p);
  std::optional<int> next_left(Rectangle p);
  std::optional<int> next_right(Rectangle p);

  void evaluate_map_object_state(IMapStateUpdatable* obj);
  void load_map(const std::string& file_path);
  [[nodiscard]] bool is_solid_tile(IntVector2D coord) const;
  [[nodiscard]] Tile& get_tile(IntVector2D coord);

  std::vector<IntVector2D> coords_of_tile_type(TileType type);

  [[nodiscard]] bool is_inside_map(IntVector2D coord) const;

  [[nodiscard]] size_t pixel_height() const;
  [[nodiscard]] size_t pixel_width() const;
};
