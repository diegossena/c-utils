#include <sdk/memory.h>
#ifdef PLATFORM_WINDOWS

#include <heapapi.h>

void* memory_alloc(u64 size) {
  assert(size > 0);
  void* block = HeapAlloc(GetProcessHeap(), 0, align(size, MEMORY_ALLOCATION_ALIGNMENT));
  __memory_count_increment(block);
  return block;
}
void* memory_alloc0(u64 size) {
  assert(size > 0);
  void* block = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, align(size, MEMORY_ALLOCATION_ALIGNMENT));
  __memory_count_increment(block);
  return block;
}
void memory_free(void* this) {
  assert(this != 0);
  HeapFree(GetProcessHeap(), 0, this);
  __memory_count_decrement();
}
void* memory_realloc(void* this, u64 size) {
  assert(size > 0);
  assert(this != null);
  return HeapReAlloc(GetProcessHeap(), 0, this, align(size, MEMORY_ALLOCATION_ALIGNMENT));
}
void* memory_realloc0(void* this, u64 size) {
  assert(size > 0);
  assert(this != null);
  return HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, this, align(size, MEMORY_ALLOCATION_ALIGNMENT));
}

#endif