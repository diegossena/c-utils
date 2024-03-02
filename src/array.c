#include "sdk/array.h"
#include "sdk/math.h"
#include "internal/memory.h"

#include <stdio.h> // TODO: remove this line

class array {
  u64 capacity;
  u64 length;
  u64 stride;
  void* data;
} array;

bool __resize(array* this, u64 capacity) {
  this->data = memory_realloc(this->data, capacity * this->stride);
  this->capacity = capacity;
}

array* _array_new(u64 stride) {
  array* this = memory_alloc0(sizeof(array));
  this->stride = stride;
  return this;
}
void array_free(array* this) {
  memory_free(this->data);
}

void* _array_at(const array* this, u64 index) {
  return this->data + index;
}
u64 array_length(const array* this) {
  return this->length;
}
u64 array_capacity(array* this) {
  return this->capacity;
}
void array_reserve(array* this, u64 size) {
  this->data = memory_realloc0(this->data, size);
  this->capacity = size;
  this->length = math_min(this->capacity, size);
}
void _array_push(array* this, const void* value) {
  if (this->length == this->capacity) {
    u64 capacity = math_next2pow(this->capacity + 1);
    __resize(this, capacity);
  }
  void* addr = this->data + this->length * this->stride;
  memory_copy(addr, value, this->stride);
  ++this->length;
}