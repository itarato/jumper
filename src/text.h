#pragma once

#include "raylib.h"
#include "shared/util.h"

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
  int vertical_offset{0};
  bool is_background{false};
  Color background_color;
  float background_rotation{0.0f};
  float background_padding{0.0f};

  explicit Text(const char* text)
      : text(text), color(DARKGRAY), font(nullptr) {}

  Text* with_color(Color new_color) {
    color = new_color;
    return this;
  }

  Text* with_background(Color color = WHITE, float rotation = 0.0f,
                        float padding = 4.0f) {
    is_background = true;
    background_color = color;
    background_rotation = rotation;
    background_padding = padding;

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

  Text* with_vertical_offset(int value) {
    vertical_offset = value;
    return this;
  }

  Text* with_aligned_center() {
    pos.x = (float)((float)GetScreenWidth() - bounds().x) / 2.0f;
    pos.y =
        (float)((float)GetScreenHeight() - bounds().y) / 2.0f + vertical_offset;
    return this;
  }

  Text* with_hover() {
    is_hover_effect = true;
    return this;
  }

  Text* with_text(const char* new_text) {
    text = new_text;
    return this;
  }

  Rectangle frame() {
    return Rectangle{
        pos.x,
        pos.y,
        (float)bounds().x,
        (float)bounds().y,
    };
  }

  void draw() {
    if (is_background) {
      Vector2 size = MeasureTextEx(*font, text, (float)font->baseSize, 0);
      Rectangle background_frame{pos.x + size.x / 2.0f, pos.y + size.y / 2.0f,
                                 size.x + background_padding * 2.0f,
                                 size.y + background_padding * 2.0f};
      DrawRectanglePro(background_frame, relative_midpoint(background_frame),
                       background_rotation, background_color);
    }

    Color draw_color;
    if (is_hover_effect &&
        CheckCollisionPointRec(GetMousePosition(), frame())) {
      draw_color = Fade(color, 0.6f);
    } else {
      draw_color = color;
    }

    DrawTextEx(*font, text, pos, (float)font->baseSize, 0, draw_color);
  }

  [[nodiscard]] Vector2 bounds() const {
    return MeasureTextEx(*font, text, (float)font->baseSize, 0);
  }
};
