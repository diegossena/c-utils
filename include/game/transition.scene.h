#ifndef TRANSITION_H
#define TRANSITION_H

#include <sdk/types.h>
#include <sdk/window.h>
#include <game/game.h>

typedef struct transition_t {
  function_t unload, load;
  // control
  bool loading;
  transition_state_t state;
  f32 timer;
} transition_t;

export void transition_load(function_t unload, function_t load);
export void transition_render();

extern transition_t transition;

#endif