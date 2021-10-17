#include "jumper.h"

#include "util.h"

#define JUMPER_WIDTH 28
#define JUMPER_HEIGHT 20

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

Rectangle Jumper::get_frame() const { return frame; }
Vector2 Jumper::get_v() const { return v; }
void Jumper::set_map_state(MapObjectState &&mos) { map_state = mos; }
