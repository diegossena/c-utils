#include <sdk/array.h>

void* array_new(u64 length) {
  array_t* this = memory_alloc0(sizeof(array_t) + length);
  assert(this);
  this->length = length;
  // this->stride = stride;
  return (void*)(this + sizeof(array_t));
}
void* array_resize(void* this, u64 length) {
  return memory_realloc0(this, sizeof(array_t) + length);
}
u64 array_length(void* this) {
  array_t* header = this - sizeof(array_t);
  return header->length;
}
void array_free(void* this) {
  array_t* header = array_header(this);
  memory_free(header);
}