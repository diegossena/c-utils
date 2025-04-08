#ifndef TRANSITION_H
#define TRANSITION_H

#include <sdk/types.h>

typedef struct transition_t {
  bool loading;
  f32 timer;
  f32 background_rect[4];
} transition_t;

export void transition_load(function_t unload, function_t load);
export void transition_update();
export void transition_draw();

extern transition_t transition;

#endif