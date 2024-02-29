#pragma once
#include "SDK/types.h"

#define MAX_BUFSIZ 32000

void* memory_alloc0(u64 size);
void* memory_alloc(u64 size);
void* memory_realloc(void* block, u64 size);
void* memory_set(void* block, byte value, u64 size);
void* memory_copy(void* dest, const void* src, u64 size);
void memory_free(void* block);