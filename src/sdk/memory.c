#include <sdk/memory.h>

i64 __leaks_count = 0;

export void memory_fill(u8* target, u8 value, u64 size) {
  while (size--)
    *target++ = value;
}
export void memory_copy(u8* target, const u8* src, u64 size) {
  while (size--)
    *target++ = *src++;
}