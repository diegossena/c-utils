#ifndef TITLESCREEN_H
#define TITLESCREEN_H

#include <sdk/types.h>
#include <sdk/keyboard.h>

typedef struct titlescreen_t {
  bool loaded;
} titlescreen_t;

export void titlescreen_load();
export void titlescreen_unload();

export void titlescreen_onkeydown(key_t key);
export void titlescreen_render();

extern titlescreen_t titlescreen;

#endif