#ifndef TITLESCREEN_H
#define TITLESCREEN_H

#include <sdk/types.h>
#include <game/game.h>

typedef struct titlescreen_t {
  bool loaded;
  // gfx_text_t title, press_space, to_play;
} titlescreen_t;

export void titlescreen_load();
export void titlescreen_unload();
export void titlescreen_render();

#endif