#include "jumper.h"

#include "asset_manager.h"
#include "util.h"

#define JUMPER_WIDTH 28
#define JUMPER_HEIGHT 20

/**
 * - Jumper pos represents bottom left corner of its bounding rect.
 */

Jumper::Jumper() {}

void Jumper::draw(int scroll_offset) {
  DrawTexture(asset_manager.textures[IMG_LADYBUG], frame.x - scroll_offset,
              frame.y, WHITE);
}

void Jumper::init(Vector2 start_pos) {
  frame.x = start_pos.x;
  frame.y = start_pos.y;
  frame.width = JUMPER_WIDTH;
  frame.height = JUMPER_HEIGHT;

  v.x = 0;
  v.y = 0;
}

Rectangle Jumper::get_frame() const { return frame; }
Vector2 Jumper::get_v() const { return v; }
void Jumper::set_map_state(MapObjectState &&mos) { map_state = mos; }
