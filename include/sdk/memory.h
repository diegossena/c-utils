#ifndef SDK_MEMORY_H
#define SDK_MEMORY_H

#include <sdk/types.h>
#include <sdk/error.h>
#include <sdk/assert.h>

#ifdef SDK_DEVELOPMENT

extern i64 __leaks_count;
#define __leaks_count_increment() ++__leaks_count
#define __leaks_count_decrement() --__leaks_count

#else

#define __leaks_count_increment()
#define __leaks_count_decrement()

#endif

SDK_EXPORT void* memory_fill(void* block, u8 value, u64 size);
SDK_EXPORT void* memory_zero(void* block, u64 size);
SDK_EXPORT void* memory_copy(void* dest, const void* src, u64 size);
SDK_EXPORT void* memory_alloc(u64 size);
SDK_EXPORT void* memory_alloc0(u64 size);
SDK_EXPORT void* memory_realloc(void* this, u64 size);
SDK_EXPORT void* memory_realloc0(void* this, u64 size);
SDK_EXPORT void memory_free(void* this);

#endif