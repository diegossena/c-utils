#include <sdk/memory.h>

#ifdef DEBUG
i64 __memory_count = 0;
#endif

void memory_fill(void* target, u8 value, u64 size) {
  while (size--)
    *(u8*)target++ = value;
}
export void* memory_copy(void* target, const void* src, u64 size) {
  while (size--)
    *(u8*)target++ = *(u8*)src++;
  return target;
}