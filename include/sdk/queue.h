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

void queue_constructor(queue_t* this);
void queue_push(queue_t* this, queue_t* queue);
void queue_unshift(queue_t* head, queue_t* q);
void queue_remove(queue_t* this);
void queue_forward(queue_t* this);

bool queue_is_empty(queue_t* this);
bool queue_is_not_empty(queue_t* this);

#endif