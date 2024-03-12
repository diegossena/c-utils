#include <sdk/platform/platform.h>

#if PLATFORM_WINDOWS

#include <sdk/memory/memory.h>
#include <sdk/error/error.h>
// GetProcessHeap, HeapReAlloc, HeapAlloc, HEAP_ZERO_MEMORY
#include <heapapi.h>

static i64 memory_counter = 0;

void* memory_alloc(u64 size) {
  void* block = HeapAlloc(GetProcessHeap(), 0, size);
  if (block) {
    ++memory_counter;
  } else {
    error("HeapAlloc", ERR_NOT_ENOUGH_MEMORY);
  }
  return block;
}
void* memory_alloc0(u64 size) {
  void* block = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
  if (block) {
    ++memory_counter;
  } else {
    error("HeapAlloc", ERR_NOT_ENOUGH_MEMORY);
  }
  return block;
}
void memory_free(void* this) {
  HeapFree(GetProcessHeap(), 0, this);
  --memory_counter;
}
void* memory_realloc(void* this, u64 size) {
  if (this) {
    this = HeapReAlloc(GetProcessHeap(), 0, this, size);
  } else {
    this = HeapAlloc(GetProcessHeap(), 0, size);
    ++memory_counter;
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
    ++memory_counter;
  }
  if (!this) {
    error("HeapReAlloc", ERR_NOT_ENOUGH_MEMORY);
  }
  return this;
}

#endif