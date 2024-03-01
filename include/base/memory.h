#pragma once
#include "SDK/types.h"

#define MAX_BUFSIZ 25565
#define memory_realloc(block, size) \
  (block ? _memory_realloc(block, size) : memory_alloc(size))

void* memory_alloc0(u64 size);
void* memory_alloc(u64 size);
void* _memory_realloc(void* block, u64 size);
void* memory_set(void* block, byte value, u64 size);
void* memory_copy(void* dest, const void* src, u64 size);
void memory_free(void* block);