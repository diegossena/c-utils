#ifndef TITLESCREEN_H
#define TITLESCREEN_H

#include <sdk/types.h>
#include <sdk/keyboard.h>

export void titlescreen_load();
export void titlescreen_unload();

export void titlescreen_onkeydown(key_t key);
export void titlescreen_render();

extern bool titlescreen;

#endif