#pragma once

#include <sdk/queue.h>


typedef void (*listener_t)(void* context);

typedef struct event_listener_t {
  queue_t queue;
  void* context;
  listener_t callback;
} event_listener_t;

void emitter_on(queue_t* emitter, event_listener_t* listener) {
  queue_push(emitter, &listener->queue);
}
void emitter_prepend(queue_t* emitter, event_listener_t* listener) {
  queue_unshift(emitter, &listener->queue);
}
void emitter_off(event_listener_t* listener) {
  queue_remove(&listener->queue);
}
void emitter_emit(queue_t* emitter) {
  event_listener_t* it = (event_listener_t*)emitter->next;
  while (it != (event_listener_t*)emitter) {
    event_listener_t* next = (event_listener_t*)it->queue.next;
    it->callback(it->context);
    it = next;
  }
}