#include "enemy.h"

#include "map.h"
#include "util.h"

void Enemy::init() { walker->init(frame); }

void Enemy::update(const Rectangle &player) { walker->update(frame, player); }

void Enemy::draw(int scroll_offset) const {
  DrawRectangle(frame.x - scroll_offset, frame.y, frame.width, frame.height,
                BLACK);
}

Enemy::Enemy(Rectangle frame, std::unique_ptr<IWalker> walker)
    : frame(frame), walker(std::move(walker)) {}
