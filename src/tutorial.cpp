#include "tutorial.h"

#include "defines.h"

#define TUTORIAL_TEXT_PADDING 4
#define TUTORIAL_TEXT_FONT_SIZE 10

Tutorial::Tutorial(std::string text, IntVector2D tile_coords) : text(text) {
  block_frame.x = tile_coords.x * BLOCK_SIZE;
  block_frame.y = tile_coords.y * BLOCK_SIZE;
  block_frame.width = BLOCK_SIZE;
  block_frame.height = BLOCK_SIZE;
}

void Tutorial::draw(IntVector2D scroll_offset) const {
  int text_width = MeasureText(text.c_str(), TUTORIAL_TEXT_FONT_SIZE);
  int bubble_height = TUTORIAL_TEXT_FONT_SIZE + TUTORIAL_TEXT_PADDING * 2;

  DrawRectangle(block_frame.x - scroll_offset.x,
                block_frame.y - scroll_offset.y - bubble_height,
                text_width + TUTORIAL_TEXT_PADDING * 2, bubble_height, RED);

  DrawRectangle(block_frame.x - scroll_offset.x,
                block_frame.y - scroll_offset.y, block_frame.width,
                block_frame.height, Fade(RED, 0.3f));

  DrawTriangle(
      Vector2{block_frame.x - scroll_offset.x, block_frame.y - scroll_offset.y},
      Vector2{block_frame.x - scroll_offset.x,
              block_frame.y - scroll_offset.y + 8},
      Vector2{block_frame.x - scroll_offset.x + 8,
              block_frame.y - scroll_offset.y},
      RED);

  DrawText(
      text.c_str(), block_frame.x - scroll_offset.x + TUTORIAL_TEXT_PADDING,
      block_frame.y - scroll_offset.y + TUTORIAL_TEXT_PADDING - bubble_height,
      10, WHITE);
}
