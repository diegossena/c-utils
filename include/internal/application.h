#pragma once

#include "internal/queue.h"

#include "sdk/application.h"
#include "sdk/error.h"

class application {
  u64 active_handles;
  queue* app_queue_head;
  queue* app_queue_tail;
  error_code stop_flag;
} application;

extern application app_main;

void app_shutdown();