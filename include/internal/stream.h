#pragma once

#include "sdk/types.h"

typedef struct stream_t {
  const void* context;
  byte* buffer;
  u64 length;
  u64 processed;
} stream_t;