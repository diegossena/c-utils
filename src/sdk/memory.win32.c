#include <sdk/memory.h>
#ifdef PLATFORM_WINDOWS

#include <heapapi.h>

export void* memory_alloc(u64 size) {
  assert(size > 0);
  void* block = HeapAlloc(GetProcessHeap(), 0, size);
  if (block) {
    __leaks_count_increment();
  } else {
    assert(block != 0);
  }
  return block;
}
export void* memory_alloc0(u64 size) {
  assert(size > 0);
  void* block = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
  if (block) {
    __leaks_count_increment();
  } else {
    assert(block != 0);
  }
  return block;
}
export void memory_free(void* this) {
  assert(this != 0);
  HeapFree(GetProcessHeap(), 0, this);
  __leaks_count_decrement();
}
export void* memory_realloc(void* this, u64 size) {
  assert(size > 0);
  assert(this != null);
  console_log("size %llu %llu", size, (size + MEMORY_ALLOCATION_ALIGNMENT - 1) & ~(MEMORY_ALLOCATION_ALIGNMENT - 1));
  return HeapReAlloc(
    GetProcessHeap(), 0, this,
    (size + MEMORY_ALLOCATION_ALIGNMENT - 1) & ~(MEMORY_ALLOCATION_ALIGNMENT - 1)
  );
}
export void* memory_realloc0(void* this, u64 size) {
  assert(size > 0);
  assert(this != null);
  return HeapReAlloc(
    GetProcessHeap(), HEAP_ZERO_MEMORY, this,
    (size + MEMORY_ALLOCATION_ALIGNMENT - 1) & ~(MEMORY_ALLOCATION_ALIGNMENT - 1)
  );
}

#endif