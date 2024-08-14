#ifndef SDK_QUEUE_H
#define SDK_QUEUE_H

#include <sdk/types.h>

#define EXTENDS_QUEUE(type) \
  type* next; \
  type* prev;

#define queue_foreach(type, head, as) \
  for ( \
    type* as = (type*)(head)->next, *it_next = (type*)as->queue.next; \
    (queue_t*)as != head; \
    as = it_next, it_next = (type*)as->queue.next \
  )

typedef struct queue_t queue_t;
typedef struct queue_t {
  queue_t* next;
  queue_t* prev;
} queue_t;

SDK_EXPORT void queue_constructor(queue_t* this);
SDK_EXPORT void queue_push(queue_t* this, queue_t* queue);
SDK_EXPORT void queue_unshift(queue_t* head, queue_t* q);
SDK_EXPORT void queue_remove(queue_t* this);
SDK_EXPORT void queue_forward(queue_t* this);
SDK_EXPORT bool queue_is_empty(const queue_t* this);

#endif