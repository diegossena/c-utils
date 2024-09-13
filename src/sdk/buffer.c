#include <sdk/buffer.h>

void* buffer_new(u64 size) {
  buffer_t* this = memory_alloc0(sizeof(buffer_t) + size);
  assert(this);
  this->length = size;
  return (void*)this + sizeof(buffer_t);
}
void* buffer_resize(void* this, u64 size) {
  buffer_t* header = this - sizeof(buffer_t);
  header = memory_realloc(header, sizeof(buffer_t) + size);
  if (header) {
    header->length = size;
  }
  return (void*)header + sizeof(buffer_t);
}
void* buffer_resize0(void* this, u64 size) {
  buffer_t* header = this - sizeof(buffer_t);
  header = memory_realloc0(header, sizeof(buffer_t) + size);
  if (header) {
    header->length = size;
  }
  return (void*)header + sizeof(buffer_t);
}
buffer_t* buffer_header(void* this) {
  return this - sizeof(buffer_t);
}
u64 buffer_length(void* this) {
  buffer_t* header = this - sizeof(buffer_t);
  return header->length;
}
void buffer_free(void* this) {
  buffer_t* header = this - sizeof(buffer_t);
  memory_free(header);
}