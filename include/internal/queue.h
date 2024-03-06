#pragma once

#include "sdk/queue.h"

typedef struct queue_t {
  queue_t* next;
  queue_t* prev;
} queue_t;

void queue_push(queue_t* head, queue_t*);
void queue_remove(queue_t*);