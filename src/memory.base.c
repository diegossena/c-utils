#include "internal/memory.h"

void* memory_fill(void* block, byte value, u64 size) {
  u8* c = (u8*)block;
  while (size) {
    *c = value;
    --size;
    ++c;
  }
  return block;
}
void* memory_copy(void* dest, const void* src, u64 size) {
  byte* dest_ptr = dest;
  const byte* src_ptr = src;
  while (true) {
    *dest_ptr = *src_ptr;
    if (!size)
      break;
    ++dest_ptr; ++src_ptr; --size;
  }
  return dest;
}