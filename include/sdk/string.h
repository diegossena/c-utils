#ifndef SDK_STRING_H
#define SDK_STRING_H

#include <sdk/types.h>
#include <sdk/buffer.h>

#define STRING_SMALL_SIZE 15

typedef struct string_t {
  const u64 length;
  const char* cstr;
  union {
    char __small[STRING_SMALL_SIZE + 1];
    u64 __capacity;
  };
} string_t;

SDK_EXPORT i32 cstr_format_va(char* this, u64 size, const char* format, void* va_list_p);

#endif