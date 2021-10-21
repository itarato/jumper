/**
 * Jumper
 *
 * Features:
 * - different map tiles (earth/obstacle/decoration)
 * - custom size tile type
 * - coin map type + score system
 * - enemy map type + healh
 * - map progressing system
 * - sprites
 * - limit dashes
 * - score save (to file)
 * - graphics
 * - xbox controller
 *
 * Optional features:
 * - teleport
 * - destroyable map type
 * - flying enemy
 *
 * Bug:
 * - sometimes fall through the floor when jumping
 */

#include <stdlib.h>
#include <time.h>

#include "app.h"
#include "util.h"

int main() {
  LOG_INFO("Start main");

  srand(time(NULL));

  App app{};
  app.init();
  app.loop();
}
