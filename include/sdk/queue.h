#pragma once

#include <sdk/types.h>

#define queue_foreach(type, head) \
  for ( \
    type* it = (type*)head.next, *it_next = (type*)it->queue.next; \
    it != (type*)&head; \
    it = it_next, it_next = (type*)it->queue.next \
  )

typedef struct queue_t queue_t;
typedef struct queue_t {
  queue_t* next;
  queue_t* prev;
} queue_t;

void queue_head(queue_t*);
void queue_forward(queue_t*);
void queue_backward(queue_t*);
void queue_push(queue_t* head, queue_t*);
void queue_remove(queue_t*);
void queue_unshift(queue_t* head, queue_t*);
bool queue_empty(queue_t* head);

#include <sdk/queue/inl.h>