#pragma once

#include <sdk/types.h>
#include <sdk/queue/queue.h>

typedef enum task_type {
  TASK_MAIN,

  TASK_WINDOW,

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

typedef void (*task_handle)();

typedef struct task_t {
  queue_t queue;
  task_type type;
} task_t;

#define task_register(this, app) queue_push((queue_t*)&app->__tasks, (queue_t*)this)
#define task_unregister(this) queue_remove((queue_t*)this)