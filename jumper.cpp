#include "jumper.h"

#include <string>
#include <vector>

#include "asset_manager.h"
#include "util.h"

#define JUMPER_WIDTH 28
#define JUMPER_HEIGHT 20
#define JUMPER_STEP_SLOWNESS 3
#define JUMPER_STAND_SLOWNESS 6

/**
 * - Jumper pos represents bottom left corner of its bounding rect.
 */

Jumper::Jumper()
    : move_sprite({IMG_LADYBUG_MOVE_0, IMG_LADYBUG_MOVE_1, IMG_LADYBUG_MOVE_2,
                   IMG_LADYBUG_MOVE_3},
                  JUMPER_STEP_SLOWNESS),
      stand_sprite({IMG_LADYBUG_STAND_0, IMG_LADYBUG_STAND_1,
                    IMG_LADYBUG_STAND_2, IMG_LADYBUG_STAND_3},
                   JUMPER_STAND_SLOWNESS) {}

void Jumper::draw(int scroll_offset) {
  std::string image_name;
  if (v.x == 0.0) {
    image_name = stand_sprite.current_img();
    stand_sprite.progress();
  } else {
    image_name = move_sprite.current_img();
    move_sprite.progress();
  }

  if (v.x < 0.0f) {
    is_facing_right = false;
  } else if (v.x > 0.0f) {
    is_facing_right = true;
  }

  if (is_facing_right) {
    DrawTextureRec(asset_manager.textures[image_name],
                   Rectangle{0.0f, 0.0f, -frame.width, frame.height},
                   Vector2{frame.x - scroll_offset, frame.y}, WHITE);
  } else {
    DrawTexture(asset_manager.textures[image_name], frame.x - scroll_offset,
                frame.y, WHITE);
  }
}

void Jumper::init(Vector2 start_pos) {
  frame.x = start_pos.x;
  frame.y = start_pos.y;
  frame.width = JUMPER_WIDTH;
  frame.height = JUMPER_HEIGHT;

  is_facing_right = true;

  v.x = 0.0f;
  v.y = 0.0f;
}

Rectangle Jumper::get_frame() const { return frame; }
Vector2 Jumper::get_v() const { return v; }
void Jumper::set_map_state(MapObjectState &&mos) { map_state = mos; }
