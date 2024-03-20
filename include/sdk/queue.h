#pragma once

#include <sdk/types.h>

#define queue_foreach(type, head, it, get_next) \
  for ( \
    type* it = (type*)head.next, *it_next = (type*)get_next; \
    it != (type*)&head; \
    it = it_next, it_next = (type*)get_next \
  )

typedef struct queue_t queue_t;
typedef struct queue_t {
  queue_t* next;
  queue_t* prev;
} queue_t;

void queue_head(queue_t*);
void queue_push(queue_t* head, queue_t*);
void queue_remove(queue_t*);
void queue_unshift(queue_t* head, queue_t*);

#include <sdk/queue/inl.h>