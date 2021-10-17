#include "jumper.h"

#include "util.h"

#define JUMPER_WIDTH 32
#define JUMPER_HEIGHT 64

/**
 * - Jumper pos represents bottom left corner of its bounding rect.
 */

Jumper::Jumper() {
  frame.x = 32;
  frame.y = 32;
  frame.width = JUMPER_WIDTH;
  frame.height = JUMPER_HEIGHT;
}

void Jumper::draw(int scroll_offset) {
  Rectangle draw_frame(frame);
  draw_frame.x -= scroll_offset;
  DrawRectangleRec(draw_frame, MAGENTA);
}
