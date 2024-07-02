#include <sdk/memory.h>
#ifdef PLATFORM_WINDOWS

#include <heapapi.h>

SDK_EXPORT void* memory_alloc(u64 size) {
  void* block = HeapAlloc(GetProcessHeap(), 0, size);
  if (block) {
    __leaks_count_increment();
  } else {
    error("HeapAlloc", ERR_NOT_ENOUGH_MEMORY);
  }
  return block;
}
SDK_EXPORT void* memory_alloc0(u64 size) {
  void* block = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
  if (block) {
    __leaks_count_increment();
  } else {
    error("HeapAlloc", ERR_NOT_ENOUGH_MEMORY);
  }
  return block;
}
SDK_EXPORT void memory_free(void* this) {
  HeapFree(GetProcessHeap(), 0, this);
  __leaks_count_decrement();
}
SDK_EXPORT void* memory_realloc(void* this, u64 size) {
  if (this) {
    this = HeapReAlloc(GetProcessHeap(), 0, this, size);
  } else {
    this = HeapAlloc(GetProcessHeap(), 0, size);
    __leaks_count_increment();
  }
  if (!this) {
    error("HeapReAlloc", ERR_NOT_ENOUGH_MEMORY);
  }
  return this;
}
SDK_EXPORT void* memory_realloc0(void* this, u64 size) {
  if (this) {
    this = HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, this, size);
  } else {
    this = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
    __leaks_count_increment();
  }
  if (!this) {
    error("HeapReAlloc", ERR_NOT_ENOUGH_MEMORY);
  }
  return this;
}

#endif