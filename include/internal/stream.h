#pragma once
// stream_t
#include "sdk/stream.h"
// task_t
#include "internal/task.h"

class stream_t {
  task_t task;
  i64 fd;
  u16 timeout;
} stream_t;