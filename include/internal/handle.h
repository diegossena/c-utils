#pragma once

#include "internal/queue.h"

#define HANDLE_FIELDS \
  handle_type type; \
  handle_close_callback close_cb; \
  queue handle_queue; \


typedef enum handle_type {
  HANDLE_TYPE
} handle_type;

typedef void (*handle_close_callback)(handle* handle);

typedef struct handle {
  HANDLE_FIELDS
} handle;