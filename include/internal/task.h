#pragma once
// task_t
#include "sdk/task.h"
// queue_t
#include "internal/queue.h"

typedef enum task_type {
  TASK_TCP_CONNECTING,
  TASK_TCP_WRITING,
  TASK_TCP_READING,
  TASK_TCP_CLOSING,

  TASK_TCP_SERVER_LISTENING,
  TASK_TCP_SERVER_CLOSING,

  TASK_TCP_CLIENT_WRITING,
  TASK_TCP_CLIENT_READING,
  TASK_TCP_CLIENT_CLOSING
} task_type;

typedef struct task_t {
  queue_t queue;
  task_type type;
  task_handle handle;
} task_t;

void task_register(task_t*);
void task_unregister(task_t*);