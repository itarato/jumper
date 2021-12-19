/**
 * Jumper
 *
 * Features:
 * - score save (to file)
 * - timed door!!!
 * - print dying reason (bad regex, enemy, spike, etc)
 * - pause
 * - make prepend/append clearer
 *
 * Bug:
 * - map overlay is still covered (separate it)
 * - timer runs during pause (is it ok or not?)
 *
 * Optional features:
 * - make pooping that attracts enemies for a bit:
 *  - adding next-poop ability delay
 *  - visual feedback of when next pooping is available
 * - limit dashes
 * - health
 * - teleport
 * - destroyable map type
 *
 * Ideas:
 * - pushable box?
 * - scalable screen?
 * - more traps:
 *  - toggle block (squashing down/up)
 *  - shooting block
 *  - toggling laser
 * - dark reveal mode
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
