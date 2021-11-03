/**
 * Jumper
 *
 * Features:
 * - xbox controller
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

int main() {
  LOG_INFO("Start main");

  srand(time(NULL));

  App app{};
  app.init();
  app.loop();

  LOG_INFO("End main");
}
