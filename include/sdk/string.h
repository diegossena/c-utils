#pragma once
#include <sdk/types.h>
#include <sdk/error.h>

#define string_append_cstr(this, cstr) {        \
  const char ptr[] = cstr;                       \
  string_append_str(this, ptr, sizeof(ptr) - 1); \
}

/*
```c
string str = {}

string str;
string_new(str);
```
*/
class string {
  char* data;
  u64 length;
} string;

void string_new(string*);
void string_free(string*);

i16 string_compare(const char*, const char*);
bool string_equal(const char*, const char*);
i32 string_format(char* this, const char* format, ...);
i32 string_format_v(char* this, const char* format, void* va_listp);

void string_append_str(string* this, const char* cstr, u64 length);
void string_append_char(string* this, const char);
