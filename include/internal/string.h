#pragma once

// @types, string_t
#include "sdk/string.h"

typedef struct string_t {
  u64 length;
  char* data;
} string_t;