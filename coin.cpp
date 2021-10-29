#include "coin.h"

#include "asset_manager.h"
#include "defines.h"
#include "util.h"

Coin::Coin(Vector2 pos)
    : frame({pos.x, pos.y, (float)BLOCK_SIZE, (float)BLOCK_SIZE}),
      is_collected(false) {}

void Coin::draw(int screen_offset) {
  DrawTexture(asset_manager.textures[IMG_COIN], frame.x - screen_offset,
              frame.y, WHITE);
}
