#include <sdk/memory.h>

i32 __leaks_memory = 0;

SDK_EXPORT void* memory_fill(void* block, u8 value, u64 size) {
  u8* c = (u8*)block;
  while (size) {
    *c = value;
    --size;
    ++c;
  }
  return block;
}
SDK_EXPORT void* memory_zero(void* block, u64 size) {
  void* start = block;
#ifdef PLATFORM_X64
  while (size >= sizeof(u64)) {
    *(u64*)block = 0;
    block += sizeof(u64);
    size -= sizeof(u64);
  }
  if (!size)
    return start;
#endif
  while (size >= sizeof(u32)) {
    *(u32*)block = 0;
    block += sizeof(u32);
    size -= sizeof(u32);
  }
  if (!size)
    return start;
  while (size >= sizeof(u16)) {
    *(u16*)block = 0;
    block += sizeof(u16);
    size -= sizeof(u16);
  }
  if (!size)
    return start;
  while (size >= sizeof(u8)) {
    *(u8*)block = 0;
    block += sizeof(u8);
    size -= sizeof(u8);
  }
  return start;
}
SDK_EXPORT void* memory_copy(void* dest, const void* src, u64 size) {
  void* start = dest;
#ifdef PLATFORM_X64
  while (size >= sizeof(u64)) {
    *(u64*)dest = *(u64*)src;
    dest += sizeof(u64);
    src += sizeof(u64);
    size -= sizeof(u64);
  }
#endif
  while (size >= sizeof(u32)) {
    *(u32*)dest = *(u32*)src;
    dest += sizeof(u32);
    src += sizeof(u32);
    size -= sizeof(u32);
  }
  while (size >= sizeof(u16)) {
    *(u16*)dest = *(u16*)src;
    dest += sizeof(u16);
    src += sizeof(u16);
    size -= sizeof(u16);
  }
  while (size >= sizeof(u8)) {
    *(u8*)dest = *(u8*)src;
    dest += sizeof(u8);
    src += sizeof(u8);
    size -= sizeof(u8);
  }
  return start;
}