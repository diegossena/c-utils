#include "base/platform.h"

#if PLATFORM_WINDOWS

#include "sdk/application.h"
#include "sdk/error.h"
#include "base/memory.h"

#include <heapapi.h>
#include <stdio.h> // TODO: remove this line

void* memory_alloc(u64 size) {
  void* block = HeapAlloc(GetProcessHeap(), 0, size);
  if (!block) {
    error("HeapAlloc", ERR_NOT_ENOUGH_MEMORY);
  }
  return block;
}
void* memory_alloc0(u64 size) {
  void* block = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
  if (!block) {
    error("HeapAlloc", ERR_NOT_ENOUGH_MEMORY);
  }
  return block;
}
void memory_free(void* this) {
  HeapFree(GetProcessHeap(), 0, this);
}
void* _memory_realloc(void* this, u64 size) {
  this = HeapReAlloc(GetProcessHeap(), 0, this, size);
  if (!this) {
    error("HeapReAlloc", ERR_NOT_ENOUGH_MEMORY);
  }
  return this;
}

#endif