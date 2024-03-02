#pragma once

#include "internal/queue.h"

#include "sdk/application.h"
#include "sdk/error.h"

class application {
  u64 active_handles;
  queue handle_queue;
  error_code stop_flag;
} application;

void app_shutdown();