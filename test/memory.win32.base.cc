#include "base/platform.h"

#if PLATFORM_WINDOWS

#include "base/memory.h"

#include <heapapi.h>

void* memory_alloc0(u64 size) {
  return (void*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
}
void* memory_alloc(u64 size) {
  return (void*)HeapAlloc(GetProcessHeap(), 0, size);
}
void memory_free(void* block) {
  HeapFree(GetProcessHeap(), 0, block);
}
void* memory_realloc(void* block, u64 size) {
  if (block) {
    return HeapReAlloc(GetProcessHeap(), 0, block, size);
  } else {
    return HeapAlloc(GetProcessHeap(), 0, size);
  }
}

#endif