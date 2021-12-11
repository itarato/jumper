#include "collectible.h"

#include "asset_manager.h"
#include "defines.h"

// COLLECTIBLE ////

Collectible::Collectible(Vector2 pos)
    : frame({pos.x, pos.y, (float)BLOCK_SIZE, (float)BLOCK_SIZE}),
      is_collected(false) {}

// COIN ////

void Coin::draw(IntVector2D screen_offset) const {
  DrawTexture(asset_manager.textures[IMG_COIN], frame.x - screen_offset.x,
              frame.y - screen_offset.y, WHITE);
}

// SHIELD ////

void Shield::draw(IntVector2D screen_offset) const {
  DrawTexture(asset_manager.textures[IMG_SHIELD], frame.x - screen_offset.x,
              frame.y - screen_offset.y, WHITE);
}
