#include "jumper.h"

#define JUMPER_WIDTH 32
#define JUMPER_HEIGHT 64

void Jumper::draw() {
  DrawRectangle(pos.x, pos.y, JUMPER_WIDTH, JUMPER_HEIGHT, MAGENTA);
}
