#pragma once

#include <sdk/types.h>
#include <sdk/queue.h>

typedef struct application_t {
  queue_t __tasks; // event_listener_t
} application_t;

static u64 __application_max_fd = 0;

void app_constructor(application_t*);
// run main loop and free on exit
i32 app_run(application_t*);

#include <sdk/application/inl.h>