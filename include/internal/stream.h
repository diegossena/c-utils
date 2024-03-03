#pragma once

#include "internal/application.h"
#include "internal/app.task.h"
#include "sdk/stream.h"

class stream_t {
  app_task_t task;
  net_connect_callback connection_cb;
  i64 fd;
  u16 timeout;
} stream_t;

// void stream_read(stream*);
// void stream_write(stream*);