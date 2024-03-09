#pragma once

#include "sdk/window/gfx/text_style.h"
#include "internal/gfx.win32.h"

typedef struct text_style_t {
  IDWriteTextFormat* format;
  ID2D1Brush* brush;
} text_style_t;