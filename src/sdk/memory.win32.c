#include <sdk/memory.h>
#ifdef PLATFORM_WINDOWS

#include <heapapi.h>

#define ALIGNED_SIZE(size) \
  ((size + MEMORY_ALLOCATION_ALIGNMENT - 1) & ~(MEMORY_ALLOCATION_ALIGNMENT - 1))

export void* memory_alloc(u64 size) {
  assert(size > 0);
  void* block = HeapAlloc(GetProcessHeap(), 0, ALIGNED_SIZE(size));
  __memory_count_increment(block);
  return block;
}
export void* memory_alloc0(u64 size) {
  assert(size > 0);
  void* block = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, ALIGNED_SIZE(size));
  __memory_count_increment(block);
  return block;
}
export void memory_free(void* this) {
  assert(this != 0);
  HeapFree(GetProcessHeap(), 0, this);
  __memory_count_decrement();
}
export void* memory_realloc(void* this, u64 size) {
  assert(size > 0);
  assert(this != null);
  return HeapReAlloc(GetProcessHeap(), 0, this, ALIGNED_SIZE(size));
}
export void* memory_realloc0(void* this, u64 size) {
  assert(size > 0);
  assert(this != null);
  return HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, this, ALIGNED_SIZE(size));
}

#undef ALIGNED_SIZE

#endif