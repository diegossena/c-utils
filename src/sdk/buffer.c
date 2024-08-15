#include <sdk/buffer.h>

buffer_t buffer_new(u64 size) {
  buffer_t buffer;
  buffer.data = memory_alloc0(size);
  assert(buffer.data);
  buffer.length = size;
  return buffer;
}
void buffer_free(buffer_t* this) {
  assert(this->data);
  memory_free((u8*)this->data);
}