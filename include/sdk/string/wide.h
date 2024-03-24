#pragma once
#pragma once
#include <sdk/types.h>
#include <sdk/error.h>

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

void wstring_new(wstring_t*);
void wstring_free(wstring_t*);

const wchar_t* wstring_cwstr(const wstring_t*);
u64 wstring_length(const wstring_t*);

void wstring_append_wstring(wstring_t*, wstring_t*);
void wstring_append_wstr(wstring_t* this, const wchar_t* cstr, u64 length);
void wstring_append_wchar(wstring_t* this, const wchar_t);
void wstring_clear(wstring_t* this);

bool wstring_equal_cwstr(const wstring_t*, const wchar_t*);
bool wstring_equal_wstring(const wstring_t*, const wstring_t*);
i16 wstring_compare(const wstring_t*, const wstring_t*);
void string_format(wstring_t*, const wchar_t* format, ...);

i16 cwstr_compare(const wchar_t*, const wchar_t*);
bool cwstr_equal(const wchar_t*, const wchar_t*);
i32 cwstr_format(wchar_t*, const wchar_t* format, ...);
u64 cwstr_length(const wchar_t*);

#include <sdk/string/wide/inl.h>

#undef SMALL_STRING_SIZE