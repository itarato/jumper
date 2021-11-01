#include "coin.h"

#include "asset_manager.h"
#include "defines.h"

Coin::Coin(Vector2 pos)
    : frame({pos.x, pos.y, (float) BLOCK_SIZE, (float) BLOCK_SIZE}),
      is_collected(false) {}

void Coin::draw(IntVector2D screen_offset) const {
  DrawTexture(asset_manager.textures[IMG_COIN], frame.x - screen_offset.x,
              frame.y - screen_offset.y, WHITE);
}
