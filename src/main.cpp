/**
 * Jumper
 *
 * Features:
 * - make pooping that attracts enemies for a bit
 *
 * Optional features:
 * - door animation
 * - regex catch animation
 * - score save (to file)
 * - limit dashes
 * - health
 * - teleport
 * - destroyable map type
 *
 * Bug:
 * - sometimes fall through the floor when jumping (limited falling velocity, lets see now)
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
