#include <sdk/array.h>

void* array_new(u64 length, u64 stride) {
  u64 array_size = length * stride;
  array_t* this = memory_alloc0(sizeof(array_t) + array_size);
  assert(this);
  this->capacity = length;
  this->stride = stride;
  return (void*)(this + sizeof(array_t));
}

array_t* array_header(void* this) {
  return this - sizeof(array_t);
}
void array_free(void* this) {
  array_t* header = array_header(this);
  memory_free(header);
}