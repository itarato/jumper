#include "enemy.h"

#include "defines.h"
#include "shared/util.h"

void Enemy::init() { walker->init(frame); }

void Enemy::update(const Rectangle& player) { walker->update(frame, player); }

void Enemy::draw(IntVector2D scroll_offset) {
  Color color{WHITE};

  if (paralyzed.get()) {
    color = Fade(WHITE, paralyzed_phaser.value());
  }

  // Todo: Use the chaser enemy texture.
  DrawTexture(*texture, (int)frame.x - scroll_offset.x,
              (int)frame.y - scroll_offset.y, color);
}

Enemy::Enemy(Rectangle frame, std::unique_ptr<IWalker> walker,
             Texture2D* texture)
    : frame(frame), walker(std::move(walker)), texture(texture) {}
