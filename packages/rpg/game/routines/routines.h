#pragma once

#include <sdk/types.h>
#include <sdk/queue.h>

typedef struct routine_t routine_t;

typedef void (*routine_start_cb)(routine_t*);
typedef void (*routine_update_cb)(routine_t*, f32 elapsed_time);

typedef struct routine_t {
  queue_t queue;
  bool started;
  bool completed;
  routine_start_cb start;
  routine_update_cb update;
} routine_t;