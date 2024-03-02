#pragma once

#include <sdk/types.h>

#define array_push(this, value) { \
  typeof(value) tmp = value;      \
  _array_push(this, &tmp);         \
}
#define array_new(type) \
  _array_new(sizeof(type))
#define array_at(this, type, index) (*(type*)_array_at(this, index))


class array array;

array* _array_new(u64 stride);
void array_free(array*);

void* _array_at(const array*, u64 index);
u64 array_length(const array*);
void array_reserve(array*, u64 size);
u64 array_capacity(array*);
void _array_push(array*, const void* value);