#pragma once

#include <sdk/types.h>

typedef struct queue_t queue_t;
typedef struct queue_t {
  queue_t* next;
  queue_t* prev;
} queue_t;

void queue_push(queue_t* head, queue_t*);
void queue_remove(queue_t*);

#include <sdk/queue/queue.inl.h>