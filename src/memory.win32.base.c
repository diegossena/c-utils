#include "internal/platform.h"

#if PLATFORM_WINDOWS

#include "sdk/application.h"
#include "sdk/error.h"
#include "internal/memory.h"

#include <heapapi.h>
#include <stdio.h> // TODO: remove this line

u64 allocated_count = 0;

void* memory_alloc(u64 size) {
  void* block = HeapAlloc(GetProcessHeap(), 0, size);
  printf("ALLOCATED_COUNT=%d\n", ++allocated_count);
  if (!block) {
    error("HeapAlloc", ERR_NOT_ENOUGH_MEMORY);
  }
  return block;
}
void* memory_alloc0(u64 size) {
  void* block = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
  printf("ALLOCATED_COUNT=%d\n", ++allocated_count);
  if (!block) {
    error("HeapAlloc", ERR_NOT_ENOUGH_MEMORY);
  }
  return block;
}
void memory_free(void* this) {
  HeapFree(GetProcessHeap(), 0, this);
  printf("ALLOCATED_COUNT=%d\n", --allocated_count);
}
void* memory_realloc(void* this, u64 size) {
  if (this) {
    this = HeapReAlloc(GetProcessHeap(), 0, this, size);
  } else {
    this = HeapAlloc(GetProcessHeap(), 0, size);
    printf("ALLOCATED_COUNT=%d\n", ++allocated_count);
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
    printf("ALLOCATED_COUNT=%d\n", ++allocated_count);
  }
  if (!this) {
    error("HeapReAlloc", ERR_NOT_ENOUGH_MEMORY);
  }
  return this;
}

#endif