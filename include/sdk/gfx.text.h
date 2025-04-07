#ifndef GFX_TEXT_H
#define GFX_TEXT_H

#include <sdk/types.h>
#include <sdk/window.h>
#include <sdk/gfx.color.h>

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
typedef void gfx_text_style_t;
typedef struct gfx_text_t {
  const wchar_t text[TINY_SIZE];
  f32 rect[4]; // [left, top, right, bottom]
  const gfx_text_style_t* style;
  const gfx_color_t* color;
} gfx_text_t;

typedef void gfx_font_t;

export void gfx_text_draw(const gfx_text_t* this);
export void gfx_text_adjust(gfx_text_t* this);

export void gfx_font_load(const wchar_t** paths, u64 count);

export gfx_text_style_t* gfx_text_style_new(const char* family, f32 size, font_weight_t weight, font_style_t style);
export void gfx_text_style_free(gfx_text_style_t* this);

#endif