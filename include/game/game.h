#ifndef GAME_H
#define GAME_H

#include <game/scenes/titlescreen.h>

typedef struct game_t {
  // gfx_color_t white;
  titlescreen_t titlescreen;
  bool repaint;
} game_t;

extern game_t state;

export void game_load();

#endif