#pragma once

#include "internal/queue.h"

#include "sdk/application.h"
#include "sdk/error.h"

class application {
  u64 active_handles;
  queue_t* app_queue_head;
  queue_t* app_queue_tail;
  error_code stop_flag;
  u64 max_fd;
} application;

extern application app_global;