#ifndef SDK_EVENTS_H
#define SDK_EVENTS_H

#include <sdk/queue.h>

typedef void (*listener_t)(void* context);

typedef struct callback_t {
  listener_t callback;
  void* context;
} callback_t;

typedef struct event_listener_t {
  queue_t queue;
  listener_t callback;
  void* context;
} event_listener_t;

void emitter_on(queue_t*, event_listener_t*);
void emitter_prepend(queue_t*, event_listener_t*);
void emitter_forward(event_listener_t*);
void emitter_off(event_listener_t*);
void emitter_emit(queue_t*);

#endif