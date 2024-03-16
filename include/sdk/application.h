#pragma once
#include <sdk/types.h>
#include <sdk/queue/queue.h>

#define task_register(this, app) queue_push((queue_t*)&app->__tasks, (queue_t*)this)
#define task_unregister(this) queue_remove((queue_t*)this)

typedef enum task_type {
  TASK_MAIN,
#ifdef SDK_WINDOW_H
  TASK_WINDOW,
#endif
#ifdef SDK_NET_TCP_H
  TASK_TCP_CONNECTING,
  TASK_TCP_WRITING,
  TASK_TCP_READING,
  TASK_TCP_CLOSING,
#endif
#ifdef SDK_NET_TCP_SERVER_H
  TASK_TCP_SERVER_LISTENING,
  TASK_TCP_SERVER_CLOSING,
  TASK_TCP_CLIENT_WRITING,
  TASK_TCP_CLIENT_READING,
  TASK_TCP_CLIENT_CLOSING
#endif
} task_type;

typedef struct task_t {
  queue_t queue;
  task_type type;
} task_t;

typedef struct application_t {
  task_t __tasks;
} application_t;

typedef void (*task_handle)();

static u64 __application_max_fd = 0;

void app_constructor(application_t*);
// run main loop and free on exit
i32 app_run(application_t*);

#include <sdk/application/application.inl.h>