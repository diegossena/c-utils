#ifndef TITLESCREEN_H
#define TITLESCREEN_H

#include <sdk/types.h>
#include <sdk/keyboard.h>
#include <sdk/gfx.text.h>
#include <sdk/gfx.image.h>

typedef struct titlescreen_t {
  bool loaded;
  gfx_color_t* white;
  gfx_text_style_t* title_style, * press_space_style;
} titlescreen_t;

export void titlescreen_load();
export void titlescreen_unload();

export void titlescreen_onkeydown(key_code_t key);
export void titlescreen_render();

extern titlescreen_t titlescreen;

#endif