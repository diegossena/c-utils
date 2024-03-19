#pragma once

#include <sdk/types.h>
#include <sdk/queue.h>

typedef struct game_t game_t;
typedef struct routine_t routine_t;

typedef struct routine_t {
  queue_t queue;
  game_t* context;
  bool completed;
  void (*render)(routine_t*);
} routine_t;

void routines_run(queue_t* routines) {
  routine_t* it = (routine_t*)routines->next;
  while (it != (routine_t*)it->queue.next) {
    it->render(it);
    it = (routine_t*)it->queue.next;
  }
}