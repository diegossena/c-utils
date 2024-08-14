#ifndef SDK_EVENTS_H
#define SDK_EVENTS_H

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

SDK_EXPORT void callback_emit(queue_t* this);

SDK_EXPORT void listener_on(queue_t* this, listener_t* listener);
SDK_EXPORT void listener_prepend(queue_t* this, listener_t* listener);
SDK_EXPORT void listener_forward(listener_t* listener);
SDK_EXPORT void listener_off(listener_t* listener);
SDK_EXPORT void listener_emit(queue_t* this, void* argument);

#endif