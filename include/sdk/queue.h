#ifndef QUEUE_H
#define QUEUE_H

#include <sdk/types.h>

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

export void queue_constructor(queue_t* this);
export void queue_push(queue_t* this, queue_t* queue);
export void queue_unshift(queue_t* head, queue_t* q);
export void queue_remove(queue_t* this);
export void queue_forward(queue_t* this);
export bool queue_is_empty(const queue_t* this);

#endif