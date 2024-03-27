#ifndef SDK_STRING_WIDE_H
#define SDK_STRING_WIDE_H

#include <sdk/types.h>
#include <sdk/error.h>
#include <sdk/memory.h>
#include <sdk/math.h>

#define SMALL_STRING_SIZE 15

#define wstring_append_cwstr(this, cwstr) { \
  const wchar_t ptr[] = cwstr; \
  wstring_append_wstr(this, ptr, sizeof(ptr) / sizeof(wchar_t) - 1); \
}

/*
```c
string str = {}

wstring_t str* =string_new(str);
```
*/
typedef struct wstring_t {
  u64 __length;
  wchar_t* __data;
  union {
    wchar_t __small_string[SMALL_STRING_SIZE + 1];
    u64 __capacity;
  };
} wstring_t;

#endif