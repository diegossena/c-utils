#include <sdk/platform.h>

#ifdef PLATFORM_WINDOWS

#include <sdk/memory.h>
#include <sdk/error.h>

void* memory_alloc(u64 size) {
  void* block = HeapAlloc(GetProcessHeap(), 0, size);
  if (block) {
#ifdef SDK_DEVELOPMENT
    ++memory_leaks;
#endif
  } else {
    error("HeapAlloc", ERR_NOT_ENOUGH_MEMORY);
  }
  return block;
}
void* memory_alloc0(u64 size) {
  void* block = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
  if (block) {
#ifdef SDK_DEVELOPMENT
    ++memory_leaks;
#endif
  } else {
    error("HeapAlloc", ERR_NOT_ENOUGH_MEMORY);
  }
  return block;
}
void memory_free(void* this) {
  HeapFree(GetProcessHeap(), 0, this);
#ifdef SDK_DEVELOPMENT
  --memory_leaks;
#endif
}
void* memory_realloc(void* this, u64 size) {
  if (this) {
    this = HeapReAlloc(GetProcessHeap(), 0, this, size);
  } else {
    this = HeapAlloc(GetProcessHeap(), 0, size);
#ifdef SDK_DEVELOPMENT
    ++memory_leaks;
#endif
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
#ifdef SDK_DEVELOPMENT
    ++memory_leaks;
#endif
  }
  if (!this) {
    error("HeapReAlloc", ERR_NOT_ENOUGH_MEMORY);
  }
  return this;
}

#endif