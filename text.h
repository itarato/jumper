#pragma once

#include "raylib.h"

/**
 * Text manifestation requires initialized window env -> needs an `.init()`
 * after window is loaded.
 */

struct Text {
  const char* text;
  Color color;
  int font_size;
  Vector2 pos;
  bool is_hover_effect;

  Text(const char* text) : text(text), color(DARKGRAY), font_size(30) {}

  Text* with_color(Color new_color) {
    color = new_color;
    return this;
  }

  Text* with_font_size(int new_font_size) {
    font_size = new_font_size;
    return this;
  }

  Text* with_pos(Vector2 new_pos) {
    pos = new_pos;
    return this;
  }

  Text* with_aligned_center() {
    pos.x = (float)(GetScreenWidth() - width()) / 2.0f;
    pos.y = (float)(GetScreenHeight() - font_size) / 2.0f;
    return this;
  }

  Text* with_hover() {
    is_hover_effect = true;
    return this;
  }

  Rectangle frame() {
    return Rectangle{
        pos.x,
        pos.y,
        (float)width(),
        (float)font_size,
    };
  }

  void draw() {
    Color draw_color;
    if (is_hover_effect &&
        CheckCollisionPointRec(GetMousePosition(), frame())) {
      draw_color = Fade(color, 0.6f);
    } else {
      draw_color = color;
    }

    DrawText(text, pos.x, pos.y, font_size, draw_color);
  }

  int width() { return MeasureText(text, font_size); }
};
