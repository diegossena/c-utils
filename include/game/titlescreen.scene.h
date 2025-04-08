#ifndef TITLESCREEN_H
#define TITLESCREEN_H

#include <sdk/types.h>
#include <sdk/keyboard.h>
#include <sdk/gfx.text.h>
#include <sdk/gfx.image.h>

typedef struct titlescreen_t {
  bool loaded;
  gfx_image_t pallet_town_interiors;
  gfx_text_t title, press_space, to_play;
} titlescreen_t;

export void titlescreen_load();
export void titlescreen_unload();

export void titlescreen_onkeydown(key_code_t key);
export void titlescreen_render();

extern titlescreen_t titlescreen;

#endif