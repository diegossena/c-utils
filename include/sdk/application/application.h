#pragma once
#include <sdk/types.h>
#include <sdk/application/task.h>

typedef struct application_t {
  task_t __tasks;
} application_t;
static u64 __application_max_fd = 0;

void app_constructor(application_t*);
// run main loop and free on exit
i32 app_run(application_t*);

#include <sdk/application/application.inl.h>