#pragma once
#include <sdk/types.h>
#include <sdk/error.h>

#define SMALL_STRING_SIZE 15

#define string_append_cstr(this, cstr) {        \
  const char ptr[] = cstr;                       \
  string_append_str(this, ptr, sizeof(ptr) - 1); \
}

/*
```c
string str = {}

string_t str* =string_new(str);
```
*/
typedef struct string_t {
  u64 __length;
  char* __data;
  union {
    char __small_string[SMALL_STRING_SIZE + 1];
    u64 __capacity;
  };
} string_t;

void string_constructor(string_t*);
void string_deconstructor(string_t*);

const char* string_cstr(const string_t*);
u64 string_length(const string_t*);

void string_append_string(string_t*, string_t*);
void string_append_str(string_t* this, const char* cstr, u64 length);
void string_append_char(string_t* this, const char);
void string_clear(string_t* this);

bool string_equal_cstr(const string_t*, const char*);
bool string_equal_string(const string_t*, const string_t*);
i16 string_compare(const string_t*, const string_t*);

i16 cstr_compare(const char*, const char*);
bool cstr_equal(const char*, const char*);
i32 cstr_format_v(char* this, const char* format, void* va_list_p);
i32 cstr_format(char*, const char* format, ...);
u64 cstr_length(const char*);

#include <sdk/string/string.inl.h>