#pragma once

#include "internal/queue.h"

interface handle handle;

typedef void (*handle_close_callback)(handle* handle);

typedef enum handle_type {
  HANDLE_TCP_LISTEN
} handle_type;

interface handle {
  queue handle_queue;
  handle_type type;
  handle_close_callback close_cb;
} handle;

void handle_init(handle*, handle_type);