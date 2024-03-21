#pragma once

#include <sdk/window.h>

typedef enum font_weight_t {
  FONT_WEIGHT_THIN = 100,
  FONT_WEIGHT_EXTRA_LIGHT = 200,
  FONT_WEIGHT_LIGHT = 300,
  FONT_WEIGHT_NORMAL = 400,
  FONT_WEIGHT_MEDIUM = 500,
  FONT_WEIGHT_SEMI_BOLD = 600,
  FONT_WEIGHT_BOLD = 700,
  FONT_WEIGHT_EXTRA_BOLD = 800,
  FONT_WEIGHT_BLACK = 900,
  FONT_WEIGHT_EXTRA_BLACK = 950
} font_weight_t;
typedef enum font_style_t {
  FONT_STYLE_NORMAL
} font_style_t;

/**
 * @brief This function should only be used in the onload event of the window_t object.
 */
void gfx_font_load(window_t*, const wchar_t* path);