#include "enemy.h"

#include "asset_manager.h"
#include "defines.h"
#include "shared/util.h"

Enemy::Enemy(Rectangle frame, std::unique_ptr<IWalker> walker,
             const char* sprite_image_name_format)
    : frame(frame),
      walker(std::move(walker)),
      sprite(sprite_image_name_format, 6) {}

void Enemy::init() { walker->init(frame); }

void Enemy::update(const Rectangle& player) {
  walker->update(frame, player);
  sprite.progress();
}

void Enemy::draw(IntVector2D scroll_offset) {
  Color color{WHITE};

  if (paralyzed.get()) {
    color = Fade(WHITE, paralyzed_phaser.value());
  }

  // Todo: Use the chaser enemy texture.
  DrawTexture(*sprite.current_img(), (int)frame.x - scroll_offset.x,
              (int)frame.y - scroll_offset.y, color);
}
