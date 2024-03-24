#pragma once
#include "SDK/types.h"

#define MAX_BUFSIZ 65535

static i32 memory_leaks = 0;

void* memory_alloc0(u64 size);
void* memory_alloc(u64 size);
void* memory_realloc(void* this, u64 size);
void* memory_realloc0(void* this, u64 size);
void* memory_zero(void* this, u64 size);
void* memory_fill(void* this, byte value, u64 size);
void* memory_copy(void* this, const void* src, u64 size);
void memory_free(void*);

#include <sdk/memory/inl.h>
#include <sdk/memory/win32.inl.h>