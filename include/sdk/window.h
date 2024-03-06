#pragma once

#include <sdk/types.h>

#define  WINDOW_POS_CENTERED MAX_U64

typedef enum window_flags {
  WINDOW_HIDDEN = 1,
  WINDOW_BORDELESS = 2,
  WINDOW_RESIZABLE = 4
} window_flags;

typedef struct window_opt {
  const char* name;
  i32 x, y, width, height;
  u64 flags;
  void* context;
} window_opt;

void window_inicialize(window_opt*);