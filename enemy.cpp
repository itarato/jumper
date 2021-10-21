#include "enemy.h"

#include "util.h"

void Enemy::init() { walker->init(frame); }

void Enemy::update() { walker->update(frame); }

void Enemy::draw(int draw_offset) { DrawRectangleRec(frame, BLACK); }

Enemy::Enemy() : walker(new RandomWalker) {}

Enemy::~Enemy() {
  if (walker) {
    delete walker;
    walker = nullptr;
  }
}
