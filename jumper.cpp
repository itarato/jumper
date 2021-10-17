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

void Jumper::draw() {
  // LOG_INFO("Jumper x:%.4f y:%.4f vx:%.4f vy:%.4f", pos.x, pos.y, v.x, v.y);
  DrawRectangle(pos.x, pos.y - bounds.y, bounds.x, bounds.y, MAGENTA);
}

Rectangle Jumper::frame() {
  return Rectangle{pos.x, pos.y, bounds.x, bounds.y};
}
