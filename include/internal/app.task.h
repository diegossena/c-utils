#pragma once

#include "internal/queue.h"
#include "sdk/app.task.h"

interface app_task_t app_task_t;

typedef void (*handle_close_callback)(app_task_t* handle);

typedef enum app_task_type {
  TASK_NONE,
  TASK_TCP_CONNECTING,
  TASK_TCP_LISTEN
} app_task_type;

interface app_task_t {
  queue_t handle_queue;
  app_task_type type;
  handle_close_callback close_cb;
} app_task_t;

void app_task_register(app_task_t*, app_task_type);
void app_task_unregister(app_task_t*);