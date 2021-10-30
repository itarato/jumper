#include "enemy.h"

#include "asset_manager.h"
#include "defines.h"
#include "util.h"

void Enemy::init() { walker->init(frame); }

void Enemy::update(const Rectangle &player) { walker->update(frame, player); }

void Enemy::draw(int scroll_offset) const {
  DrawTexture(asset_manager.textures[IMG_ENEMY], (int)frame.x - scroll_offset,
              (int)frame.y, WHITE);
}

Enemy::Enemy(Rectangle frame, std::unique_ptr<IWalker> walker)
    : frame(frame), walker(std::move(walker)) {}
