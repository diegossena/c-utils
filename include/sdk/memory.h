#ifndef MEMORY_H
#define MEMORY_H

#include <sdk/types.h>
#include <sdk/error.h>
#include <sdk/assert.h>

export void memory_fill(void* block, u8 value, u64 size);
export void memory_copy(void* dest, const void* src, u64 size);
export void* memory_alloc(u64 size);
export void* memory_alloc0(u64 size);
export void* memory_realloc(void* this, u64 size);
export void* memory_realloc0(void* this, u64 size);
export void memory_free(void* this);

#ifdef DEBUG

extern i64 __memory_count;
#define __memory_count_increment(block) __memory_count += block ? 1 : 0
#define __memory_count_decrement() --__memory_count

#else

#define __memory_count_increment(block)
#define __memory_count_decrement()

#endif

#endif