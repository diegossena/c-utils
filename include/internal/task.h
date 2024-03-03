#pragma once

#include "sdk/task.h"
#include "internal/queue.h"
#include "internal/queue.h"

typedef enum task_type {
  TASK_NONE,
  TASK_TCP_CONNECT,
  TASK_TCP_LISTEN
} task_type;

interface task_t {
  queue_t queue;
  task_type type;
  task_handle handle;
} task_t;

void task_register(task_t*);
void task_unregister(task_t*);
