#ifndef MEMORY_H
#define MEMORY_H

#include <sdk/types.h>
#include <sdk/error.h>
#include <sdk/assert.h>

#ifdef DEVELOPMENT

#include <sdk/console.h>

extern i64 __leaks_count;
#define __leaks_count_increment() ++__leaks_count
#define __leaks_count_decrement() --__leaks_count
#define memory_debug() \
  if(__leaks_count != 0) { \
    console_log("leaks_count %lld", __leaks_count); \
  }

#else

#define __leaks_count_increment()
#define __leaks_count_decrement()
#define memory_debug()

#endif

export void memory_fill(void* block, u8 value, u64 size);
export void memory_copy(void* dest, const void* src, u64 size);
export void* memory_alloc(u64 size);
export void* memory_alloc0(u64 size);
export void* memory_realloc(void* this, u64 size);
export void* memory_realloc0(void* this, u64 size);
export void memory_free(void* this);

#endif