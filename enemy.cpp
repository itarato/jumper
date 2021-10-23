#include "enemy.h"

#include "map.h"
#include "util.h"

void Enemy::init() { walker->init(frame); }

void Enemy::update(const Rectangle &player) { walker->update(frame, player); }

void Enemy::draw(int draw_offset) { DrawRectangleRec(frame, BLACK); }

Enemy::Enemy(Map *map) : walker(new StrictPathChaseWalker(map)) {}

Enemy::~Enemy() {
  if (walker) {
    delete walker;
    walker = nullptr;
  }
}
