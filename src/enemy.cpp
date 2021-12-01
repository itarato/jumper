#include "enemy.h"

#include "asset_manager.h"
#include "defines.h"
#include "shared/util.h"

void Enemy::init() { walker->init(frame); }

void Enemy::update(const Rectangle &player) { walker->update(frame, player); }

void Enemy::draw(IntVector2D scroll_offset) {
  Color color{WHITE};

  if (paralyzed.is_true()) {
    color = Fade(WHITE, paralyzed_phaser.value());
    paralyzed_phaser.step();
  }

  DrawTexture(asset_manager.textures[IMG_ENEMY], (int) frame.x - scroll_offset.x,
              (int) frame.y - scroll_offset.y, color);
}

Enemy::Enemy(Rectangle frame, std::unique_ptr<IWalker> walker)
    : frame(frame),
      walker(std::move(walker)) {}
