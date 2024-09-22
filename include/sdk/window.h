#ifndef SDK_WINDOW_H
#define SDK_WINDOW_H

#include <sdk/types.h>

typedef struct window_t {
  void* _handle;
} window_t;

#include <sdk/taskmanager.h>

void window_constructor(
  window_t* this,
  const char* title,
  i32 width, i32 height
);

#endif