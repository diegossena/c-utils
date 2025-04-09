#ifndef TRANSITION_H
#define TRANSITION_H

#include <sdk/types.h>

typedef struct transition_t {
  function_t unload, load;
  // control
  bool loading;
  f32 timer;
  bool fade_in_out;
} transition_t;

export void transition_load(function_t unload, function_t load);
export void transition_render();

extern transition_t transition;

#endif