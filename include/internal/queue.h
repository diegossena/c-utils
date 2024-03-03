#pragma once

#include "sdk/queue.h"

interface queue_t {
  queue_t* next;
  queue_t* prev;
} queue_t;

void queue_push(queue_t** head, queue_t*);
void queue_remove(queue_t** head, queue_t*);