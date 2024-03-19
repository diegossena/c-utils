#pragma once

#include <sdk/types.h>
#include <sdk/queue.h>

typedef struct game_t game_t;
typedef struct routine_t routine_t;

typedef struct routine_t {
  queue_t queue;
  game_t* context;
  bool completed;
  void (*update)(routine_t*);
} routine_t;
