/**
 * Jumper
 *
 * Features:
 * - score save (to file)
 *
 * Bug:
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
 * - jumper producing sugar attracting enemy for short time
 */

#include <cstdlib>
#include <ctime>

#include "app.h"
#include "asset_manager.h"
#include "game_config.h"
#include "shared/util.h"

int main(int argc, char** argv) {
  LOG_INFO("Start main");

  srand(time(nullptr));

  GameConfig game_config(parse_args(argc, argv));

  App app{std::move(game_config)};
  app.init();
  app.loop();

  LOG_INFO("End main");
}
