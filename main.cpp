/**
 * Jumper
 *
 * Features:
 * - different map tiles (earth/obstacle/decoration)
 * - custom size tile type
 * - xbox controller
 *
 * Optional features:
 * - score save (to file)
 * - limit dashes
 * - health
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
