#ifndef TITLESCREEN_H
#define TITLESCREEN_H

#include <sdk/types.h>
#include <sdk/keyboard.h>

void titlescreen_load();
void titlescreen_unload();

void titlescreen_onkeydown(key_t key);
void titlescreen_render();

extern bool titlescreen;

#endif