/**
 * Jumper
 *
 * Features:
 * - make pooping that attracts enemies for a bit:
 *  - enemy animation when paralized by poop (shake or something)
 *  - jumper struggle animation when making poop
 *  - adding next-poop ability delay
 *  - visual feedback of when next pooping is available
 *  - visual feedback of when poop disappears
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
