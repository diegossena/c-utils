#ifndef SDK_WINDOW_GFX_STROKE_H
#define SDK_WINDOW_GFX_STROKE_H

#include <sdk/window.h>

typedef enum stroke_style_t {
  STROKE_STYLE_SOLID
} stroke_style_t;

typedef struct gfx_stroke_props_t {
  const window_t* window;
  stroke_style_t style;
} gfx_stroke_props_t;

typedef struct gfx_stroke_t gfx_stroke_t;

void gfx_stroke_new(gfx_stroke_t*, gfx_stroke_props_t);
void gfx_stroke_free(gfx_stroke_t*);

#endif