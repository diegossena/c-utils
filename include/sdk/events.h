#ifndef EVENTS_H
#define EVENTS_H

#include <sdk/types.h>
#include <sdk/queue.h>

typedef struct callback_t {
  queue_t queue;
  function_t callback;
  void* this;
} callback_t;
typedef struct listener_t {
  queue_t queue;
  function_t callback;
} listener_t;

void callback_emit(queue_t* this);
void callback_on(queue_t* this, callback_t* callback);

void listener_on(queue_t* this, listener_t* listener);
void listener_prepend(queue_t* this, listener_t* listener);
void listener_forward(listener_t* listener);
void listener_off(listener_t* listener);
void listener_emit(queue_t* this, void* argument);

#endif