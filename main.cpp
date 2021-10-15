#include "app.h"
#include "raylib.h"
#include "util.h"

int main() {
  LOG_INFO("Start main");

  App app{};
  app.init();
  app.loop();
}
