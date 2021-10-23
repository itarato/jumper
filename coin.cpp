#include "coin.h"

#include "util.h"

Coin::Coin(Vector2 pos)
    : frame({pos.x, pos.y, (float)BLOCK_SIZE, (float)BLOCK_SIZE}),
      is_collected(false) {}

void Coin::draw(int screen_offset) {
  DrawRectangle(frame.x - screen_offset, frame.y, frame.width, frame.height,
                YELLOW);
}
