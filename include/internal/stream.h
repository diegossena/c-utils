#pragma once

#include "sdk/types.h"

typedef struct stream_t {
  const void* context;
  byte* buffer;
  u64 size;
  u64 length;
  u64 processed;
  u64 updatedAt;
} stream_t;