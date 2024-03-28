#ifndef SDK_WINDOW_GFX_TEXT_H
#define SDK_WINDOW_GFX_TEXT_H

#include <sdk/window/gfx/color.h>
#include <sdk/string/wide.h>

typedef struct gfx_text_style_t gfx_text_style_t;

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

typedef struct text_style_props_t {
  // required
  window_t* window;
  const wchar_t* family;
  f32 size;
  font_weight_t weight;
  // optional
  font_style_t style;
} text_style_props_t;

typedef struct gfx_text_t {
  // required
  window_t* window;
  wstring_t text;
  rect_t rect;
  const gfx_text_style_t* style;
  const gfx_color_t* color;
  // listeners
  event_listener_t __ondestroy;
} gfx_text_t;

/**
 * @brief This function should only be used in the onload event of the window_t object.
 */
void gfx_font_load(window_t*, const wchar_t* path);

void gfx_text_style_new(gfx_text_style_t* this, text_style_props_t);
void gfx_text_style_free(gfx_text_style_t*);

void gfx_text_new(gfx_text_t*);
void gfx_text_free(gfx_text_t*);
void gfx_text_adjust(gfx_text_t*);
void gfx_text_draw(const gfx_text_t*);

#endif