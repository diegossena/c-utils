#include <sdk/memory.h>

void* memory_fill(void* block, byte value, u64 size) {
  byte* c = (byte*)block;
  while (size) {
    *c = value;
    --size;
    ++c;
  }
  return block;
}
void* memory_zero(void* block, u64 size) {
  void* start = block;
  while (size >= sizeof(u64)) {
    *(u64*)block = 0;
    block += sizeof(u64);
    size -= sizeof(u64);
  }
  while (size >= sizeof(u32)) {
    *(u32*)block = 0;
    block += sizeof(u32);
    size -= sizeof(u32);
  }
  while (size >= sizeof(u16)) {
    *(u16*)block = 0;
    block += sizeof(u16);
    size -= sizeof(u16);
  }
  while (size >= sizeof(byte)) {
    *(byte*)block = 0;
    block += sizeof(byte);
    size -= sizeof(byte);
  }
  return start;
}
void* memory_copy(void* dest, const void* src, u64 size) {
  void* start = dest;
  while (size >= sizeof(u64)) {
    *(u64*)dest = *(u64*)src;
    dest += sizeof(u64);
    src += sizeof(u64);
    size -= sizeof(u64);
  }
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
  while (size >= sizeof(byte)) {
    *(byte*)dest = *(byte*)src;
    dest += sizeof(byte);
    src += sizeof(byte);
    size -= sizeof(byte);
  }
  return start;
}