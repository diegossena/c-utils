#pragma once

#include <sdk/types.h>

#define array_push(this, value) { \
  typeof(value) tmp = value;      \
  _array_push(this, &tmp);         \
}
#define array_new(this, type) \
  array_new_stride(this, sizeof(type))


class array {
  u64 capacity;
  u64 length;
  u64 stride;
  void* data;
} array;

void array_new_stride(array* this, u64 stride);
void array_free(array* this);

void array_reserve(array* this, u64 size);
void _array_push(array* this, const void* value);