#include "poop.h"

#include "asset_manager.h"
#include "defines.h"

Poop::Poop(Vector2 pos) : pos(pos) {}

void Poop::update() {
  timer--;
}

void Poop::draw(IntVector2D screen_offset) const {
  DrawTexture(asset_manager.textures[IMG_POOP], pos.x - screen_offset.x, pos.y - screen_offset.y, WHITE);
}

bool Poop::is_dead() const {
  return timer <= 0;
}

Rectangle Poop::frame() const {
  return Rectangle{
          pos.x,
          pos.y,
          BLOCK_SIZE,
          BLOCK_SIZE,
  };
}