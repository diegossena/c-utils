#include "internal/platform.h"

#if PLATFORM_WINDOWS
// error, ERR_NOT_ENOUGH_MEMORY
#include "sdk/error.h"
// memory_alloc, memory_alloc0, memory_free, memory_realloc, memory_realloc0 
#include "internal/memory.h"
// GetProcessHeap, HeapReAlloc, HeapAlloc, HEAP_ZERO_MEMORY
#include <heapapi.h>
// printf
#include <stdio.h> // TODO: remove this line

i64 memory_counter = 0;

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
void* memory_realloc(void* this, u64 size) {
  if (this) {
    this = HeapReAlloc(GetProcessHeap(), 0, this, size);
  } else {
    this = HeapAlloc(GetProcessHeap(), 0, size);
  }
  if (!this) {
    error("HeapReAlloc", ERR_NOT_ENOUGH_MEMORY);
  }
  return this;
}
void* memory_realloc0(void* this, u64 size) {
  if (this) {
    this = HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, this, size);
  } else {
    this = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
  }
  if (!this) {
    error("HeapReAlloc", ERR_NOT_ENOUGH_MEMORY);
  }
  return this;
}

#endif