#pragma once
// task_t
#include "internal/task.h"
// application
#include "sdk/application.h"
// error_code
#include "sdk/error.h"

typedef struct application {
  task_t tasks;
  u64 tasks_count;
  bool stopping;
  u64 max_fd;
} application;

extern application app_global;