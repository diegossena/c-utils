#pragma once

#include <sdk/types.h>
#include <sdk/window.h>

typedef struct text_style_t text_style_t;

typedef enum font_weight_t {
  FONT_WEIGHT_NORMAL
} font_weight_t;
typedef enum font_style_t {
  FONT_STYLE_NORMAL
} font_style_t;

typedef struct text_style_properties_t {
  const wchar_t* family;
  font_weight_t weight;
  font_style_t style;
  float size;
} text_style_properties_t;


text_style_t* text_style_new(text_style_properties_t*, window_t*);
void text_style_free(text_style_t*);