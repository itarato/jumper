#include "input.h"

#include "raylib.h"

int gamepad_button_for_key(int key) {
  switch (key) {
    case KEY_SPACE:
      return 7;
    case KEY_LEFT_CONTROL:
      return 8;
    case KEY_LEFT_ALT:
      return 6;
    case KEY_ENTER:
      return 5;
    case KEY_BACKSPACE:
      return 15;
    default:
      return 0;
  }
}

bool is_key_pressed(int key) {
  if (IsKeyPressed(key)) return true;

  int gamepad_button{gamepad_button_for_key(key)};
  if (gamepad_button > 0 && IsGamepadButtonPressed(0, gamepad_button)) return true;

  return false;
}

bool is_key_down(int key) {
  if (IsKeyDown(key)) return true;

  int gamepad_button{gamepad_button_for_key(key)};
  if (gamepad_button > 0 && IsGamepadButtonDown(0, gamepad_button)) return true;

  return false;
}

float get_horizontal_axis() {
  if (IsKeyPressed(KEY_LEFT)) return -1.0f;
  if (IsKeyPressed(KEY_RIGHT)) return 1.0f;
  return GetGamepadAxisMovement(0, 0);
}
