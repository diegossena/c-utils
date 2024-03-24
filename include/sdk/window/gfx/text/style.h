#pragma once

#include <sdk/window.h>
#include <sdk/window/gfx/font.h>

typedef struct gfx_textstyle_t gfx_textstyle_t;

typedef struct textstyle_props_t {
  // required
  const window_t* window;
  const wchar_t* family;
  f32 size;
  font_weight_t weight;
  // optional
  font_style_t style;
} textstyle_props_t;

void gfx_textstyle_new(gfx_textstyle_t* this, textstyle_props_t);
void gfx_textstyle_free(gfx_textstyle_t*);

#include <sdk/window/gfx/text/style/win32.inl.h>