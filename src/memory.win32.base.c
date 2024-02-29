#include "base/platform.h"

#if PLATFORM_WINDOWS

#include "sdk/application.h"
#include "sdk/error.h"
#include "base/memory.h"

#include <heapapi.h>

u64 allocated = 0;

void* memory_alloc(u64 size) {
  void* block = HeapAlloc(GetProcessHeap(), 0, size);
  if (!block)
    error("HeapAlloc", ERR_NOT_ENOUGH_MEMORY);
  return block;
}
void* memory_alloc0(u64 size) {
  void* block = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
  if (!block)
    error("HeapAlloc", ERR_NOT_ENOUGH_MEMORY);
  return block;
}
void memory_free(void* block) {
  HeapFree(GetProcessHeap(), 0, block);
}
void* memory_realloc(void* block, u64 size) {
  void* new_block;
  if (block) {
    new_block = HeapReAlloc(GetProcessHeap(), 0, block, size);
  } else {
    new_block = HeapAlloc(GetProcessHeap(), 0, size);
  }
  if (!new_block)
    error("HeapAlloc", ERR_NOT_ENOUGH_MEMORY);
  return new_block;
}

#endif