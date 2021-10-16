#include "coordinator.h"
#include "raylib.h"

namespace Coordinator {

void move(IMovable *movable, Vector2 delta) { movable->move(delta); }

} // namespace Coordinator
