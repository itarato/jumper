/**
 * Jumper
 *
 * Features:
 * - invisibility - that saves from enemy
 * - score save (to file)
 * - add sprite lenght discovery
 *
 * Optional features:
 * - make pooping that attracts enemies for a bit:
 *  - adding next-poop ability delay
 *  - visual feedback of when next pooping is available
 * - limit dashes
 * - health
 * - teleport
 * - destroyable map type
 */

#include <cstdlib>
#include <ctime>

#include "app.h"
#include "shared/util.h"

int main(int argc, char** argv) {
  LOG_INFO("Start main");

  srand(time(nullptr));

  App app{parse_args(argc, argv)};
  app.init();
  app.loop();

  LOG_INFO("End main");
}
