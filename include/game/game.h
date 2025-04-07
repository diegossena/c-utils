#ifndef GAME_H
#define GAME_H

#include <game/scenes/titlescreen.h>

typedef struct game_t {
  titlescreen_t titlescreen;
} game_t;

extern game_t state;

#endif