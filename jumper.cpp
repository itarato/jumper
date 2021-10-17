#include "jumper.h"

#include "util.h"

#define JUMPER_WIDTH 20
#define JUMPER_HEIGHT 48

/**
 * - Jumper pos represents bottom left corner of its bounding rect.
 */

Jumper::Jumper() { init(); }

void Jumper::draw(int scroll_offset) {
  Rectangle draw_frame(frame);
  draw_frame.x -= scroll_offset;
  DrawRectangleRec(draw_frame, MAGENTA);
}

void Jumper::init() {
  frame.x = 32;
  frame.y = 32;
  frame.width = JUMPER_WIDTH;
  frame.height = JUMPER_HEIGHT;

  v.x = 0;
  v.y = 0;
}
