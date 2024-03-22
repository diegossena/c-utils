#include <sdk/memory.h>

void* memory_fill(void* block, byte value, u64 size) {
  u8* c = (u8*)block;
  while (size) {
    *c = value;
    --size;
    ++c;
  }
  return block;
}
void* memory_copy_64(void* dest, const void* src, u64 size) {
  u64* dest_ptr = dest;
  const u64* src_ptr = src;
  while (size > 0) {
    *dest_ptr = *src_ptr;
    ++dest_ptr; ++src_ptr;
    size -= sizeof(u64);
  }
  return dest;
}
void* memory_copy_32(void* dest, const void* src, u64 size) {
  u32* dest_ptr = dest;
  const u32* src_ptr = src;
  while (size > 0) {
    *dest_ptr = *src_ptr;
    ++dest_ptr; ++src_ptr;
    size -= sizeof(u32);
  }
  return dest;
}
void* memory_copy_16(void* dest, const void* src, u64 size) {
  u16* dest_ptr = dest;
  const u16* src_ptr = src;
  while (size > 0) {
    *dest_ptr = *src_ptr;
    ++dest_ptr; ++src_ptr;
    size -= sizeof(u16);
  }
  return dest;
}
void* memory_copy_8(void* dest, const void* src, u64 size) {
  byte* dest_ptr = dest;
  const byte* src_ptr = src;
  while (size > 0) {
    *dest_ptr = *src_ptr;
    ++dest_ptr; ++src_ptr; --size;
  }
}
void* memory_copy(void* dest, const void* src, u64 size) {
  switch (size % sizeof(u64)) {
    case 0:
      return memory_copy_64(dest, src, size);
    case 4:
      return memory_copy_32(dest, src, size);
    case 2:
      return memory_copy_16(dest, src, size);
    default:
      return memory_copy_8(dest, src, size);
  }
}