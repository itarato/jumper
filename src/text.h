#pragma once

#include "raylib.h"

/**
 * Text manifestation requires initialized window env -> needs an `.init()`
 * after window is loaded.
 */

struct Text {
  const char* text;
  Color color;
  Font* font;
  Vector2 pos;
  bool is_hover_effect;

  explicit Text(const char* text) : text(text),
                                    color(DARKGRAY),
                                    font(nullptr) {}

  Text* with_color(Color new_color) {
    color = new_color;
    return this;
  }

  Text* with_font(Font* new_font) {
    font = new_font;
    return this;
  }

  Text* with_pos(Vector2 new_pos) {
    pos = new_pos;
    return this;
  }

  Text* with_aligned_center() {
    pos.x = (float) ((float) GetScreenWidth() - bounds().x) / 2.0f;
    pos.y = (float) ((float) GetScreenHeight() - bounds().y) / 2.0f;
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
            (float) bounds().x,
            (float) bounds().y,
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

    DrawTextEx(*font, text, Vector2{pos.x, pos.y}, (float) font->baseSize, 0, draw_color);
  }

  [[nodiscard]] Vector2 bounds() const { return MeasureTextEx(*font, text, (float) font->baseSize, 0); }
};
