#include "sdk/array.h"
#include "sdk/math.h"
#include "base/memory.h"

#include <stdio.h> // TODO: remove this line

bool __resize(array* this, u64 capacity) {
  this->data = memory_realloc(this->data, capacity * this->stride);
  this->capacity = capacity;
}

void array_new_stride(array* this, u64 stride) {
  this->capacity = 0;
  this->length = 0;
  this->stride = stride;
  this->data = 0;
}
void array_free(array* this) {
  memory_free(this->data);
}

void array_reserve(array* this, u64 size) {

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