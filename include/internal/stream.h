#pragma once

#include "internal/application.h"
#include "internal/handle.h"

#define STREAM_FIELDS \
  u64 write_queue_size; \
  stream_alloc_callback alloc_callback; \
  stream_read_callback read_callback; \
  u64 reqs_pending; \
  i32 activecnt; \
  stream_read_t read_req;

typedef void (*stream_alloc_callback)();
typedef void (*stream_read_callback)();

typedef struct stream_read_t {
  void* event_handle;
  void* wait_handle;
} stream_read_t;

typedef struct stream {
  HANDLE_FIELDS;
  STREAM_FIELDS;
} stream;

void stream_read(stream*);
void stream_write(stream*);