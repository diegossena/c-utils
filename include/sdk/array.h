#pragma once

#include <sdk/types.h>

#define array_push(this, value) { \
  typeof(value) tmp = value;      \
  _array_push(this, &tmp);         \
}
#define array_new(type) \
  _array_new(sizeof(type))
#define array_at(this, type, index) \
  (*(type*)_array_at(this, index))


typedef struct array_t array_t;

array_t* _array_new(u64 stride);
void array_free(array_t*);

void* _array_at(const array_t*, u64 index);
u64 array_length(const array_t*);
void array_reserve(array_t*, u64 size);
u64 array_capacity(array_t*);
void _array_push(array_t*, const void* value);