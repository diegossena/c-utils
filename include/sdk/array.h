#pragma once

#include <sdk/types.h>

#define array_push(this, value) { \
  typeof(value) tmp = value;      \
  _array_push(this, &tmp);         \
}
#define array_constructor(this, type) \
  _array_constructor(this, sizeof(type))


class array {
  u64 capacity;
  u64 length;
  u64 stride;
  void* data;
} array;

void _array_constructor(array*, u64 stride);
void array_free(array*);

void _array_push(array*, const void* value);