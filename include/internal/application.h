#pragma once
// queue_t
#include "internal/queue.h"
// application
#include "sdk/application.h"
// error_code
#include "sdk/error.h"

typedef struct application {
  u64 active_handles;
  queue_t* tasks;
  error_code stop_flag;
  u64 max_fd;
} application;

extern application app_global;