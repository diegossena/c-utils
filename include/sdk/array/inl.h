#include <sdk/array.h>

#include <sdk/memory.h>

bool __array_resize(array_t* this, u64 capacity) {
  this->__data = memory_realloc(this->__data, capacity * this->__stride);
  this->__capacity = capacity;
}

void _array_constructor(array_t* this, u64 stride) {
  this->__length = 0;
  this->__capacity = 0;
  this->__data = 0;
  this->__stride = stride;
}
void array_deconstructor(array_t* this) {
  memory_free(this->__data);
}

void* _array_at(const array_t* this, u64 index) {
  return this->__data + this->__stride * index;
}
u64 array_length(const array_t* this) {
  return this->__length;
}
u64 array_capacity(array_t* this) {
  return this->__capacity;
}
void array_reserve(array_t* this, u64 size) {
  this->__data = memory_realloc0(this->__data, size);
  this->__capacity = size;
  this->__length = math_min(this->__capacity, size);
}
void _array_push(array_t* this, const void* value) {
  if (this->__length == this->__capacity) {
    u64 capacity = math_next2pow(this->__capacity + 1);
    __array_resize(this, capacity);
  }
  void* addr = this->__data + this->__length * this->__stride;
  memory_copy(addr, value, this->__stride);
  ++this->__length;
}