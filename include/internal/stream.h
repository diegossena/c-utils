#pragma once

#include "internal/application.h"
#include "internal/handle.h"

typedef void (*stream_alloc_callback)();
typedef void (*stream_read_callback)();

typedef struct stream_read_t {
  void* event_handle;
  void* wait_handle;
} stream_read_t;

typedef struct stream {
  handle stream_handle;

  u64 write_queue_size;
  stream_alloc_callback alloc_callback;
  stream_read_callback read_callback;
  u64 reqs_pending;
  i32 activecnt;
  stream_read_t read_req;
} stream;

void stream_init(stream*, handle_type);

void stream_read(stream*);
void stream_write(stream*);