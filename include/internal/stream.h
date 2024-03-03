#pragma once

#include "internal/application.h"
#include "internal/handle.h"
#include "sdk/stream.h"

class stream {
  handle stream_handle;
  net_connect_callback connection_cb;
} stream;

void stream_init(stream*, handle_type);

// void stream_read(stream*);
// void stream_write(stream*);