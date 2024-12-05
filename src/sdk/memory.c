#include <sdk/memory.h>

i64 __leaks_count = 0;

export void memory_fill(void* target, u8 value, u64 size) {
  while (size--)
    *(u8*)target++ = value;
}
export void memory_copy(void* target, const void* src, u64 size) {
  while (size--)
    *(u8*)target++ = *(u8*)src++;
}