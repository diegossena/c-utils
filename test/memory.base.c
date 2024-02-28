#include "base/memory.h"

void* memory_set(void* block, i64 value, u64 size) {
  u8* c = (u8*)block;
  while (size) {
    *c = value;
    --size;
    ++c;
  }
  return block;
}
void* memory_copy(void* dest, const void* src, u64 size) {
  u8* dest_ptr = (u8*)dest;
  u8* src_ptr = (u8*)src;
  while (size) {
    *dest_ptr = *src_ptr;
    --size;
    ++dest_ptr;
    ++src_ptr;
  }
  return dest;
}