#pragma once

typedef struct window_t window_t;

#define gfx_draw_text_cwstr(this, text, props) { \
  const wchar_t __text[] = text; \
  gfx_draw_text(this, text, sizeof(text) / sizeof(wchar_t) - 1, props); \
}

typedef enum font_weight_t {
  FONT_WEIGHT_NORMAL,
  FONT_WEIGHT_BOLD
} font_weight_t;
typedef enum font_style_t {
  FONT_STYLE_NORMAL
} font_style_t;
/**
 * top left corner [0, 0]
 * bottom right corner [window.height, window.width]
 */
typedef struct gfx_rect_t { f32 left, top, right, bottom; } gfx_rect_t;
typedef struct gfx_color_t { f32 r, g, b, a; } gfx_color_t;
typedef struct bitmap_t bitmap_t;

typedef struct text_props_t {
  const wchar_t* family;
  font_weight_t weight;
  font_style_t style;
  float size;
  gfx_rect_t rect;
  gfx_color_t color;
} text_props_t;

typedef struct rect_props_t {
  gfx_rect_t rect;
  gfx_color_t color;
} rect_props_t;
typedef struct ellipse_props_t {
  float x, y;
  float radius_x, radius_y;
  gfx_color_t color;
} ellipse_props_t;
typedef struct bitmap_props_t {
  gfx_rect_t rect;
  bitmap_t* image;
} bitmap_props_t;

void rect_set_width(gfx_rect_t*, f32);
void rect_set_height(gfx_rect_t*, f32);
void rect_set_size(gfx_rect_t*, f32);

void gfx_draw_text(window_t*, const wchar_t* text, u64 length, text_props_t*);
void gfx_draw_rect(window_t*, rect_props_t*);
void gfx_draw_ellipse(window_t*, ellipse_props_t*);
void gfx_draw_bitmap(window_t*, bitmap_props_t*);

void gfx_bitmap_constructor(bitmap_t*, window_t*, const wchar_t* path);
void gfx_bitmap_deconstructor(bitmap_t*);

#include <sdk/window/gfx.win32.inl.h>
#include <sdk/window/gfx.inl.h>