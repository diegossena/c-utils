#include <sdk/memory.h>
#ifdef PLATFORM_WINDOWS

#include <heapapi.h>

export void* memory_alloc(u64 size) {
  assert(size > 0);
  void* block = HeapAlloc(GetProcessHeap(), 0, size);
  if (block) {
    __leaks_count_increment();
  } else {
    error_log("HeapAlloc", ERR_NOT_ENOUGH_MEMORY);
  }
  return block;
}
export void* memory_alloc0(u64 size) {
  assert(size > 0);
  void* block = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
  if (block) {
    __leaks_count_increment();
  } else {
    error_log("HeapAlloc", ERR_NOT_ENOUGH_MEMORY);
  }
  return block;
}
export void memory_free(void* this) {
  assert(this);
  HeapFree(GetProcessHeap(), 0, this);
  __leaks_count_decrement();
}
export void* memory_realloc(void* this, u64 size) {
  assert(size > 0);
  if (this) {
    this = HeapReAlloc(GetProcessHeap(), 0, this, size);
  } else {
    this = HeapAlloc(GetProcessHeap(), 0, size);
    __leaks_count_increment();
  }
  if (!this) {
    error_log("HeapReAlloc", ERR_NOT_ENOUGH_MEMORY);
  }
  return this;
}
export void* memory_realloc0(void* this, u64 size) {
  assert(size > 0);
  if (this) {
    this = HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, this, size);
  } else {
    this = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
    __leaks_count_increment();
  }
  if (!this) {
    error_log("HeapReAlloc", ERR_NOT_ENOUGH_MEMORY);
  }
  return this;
}

#endif