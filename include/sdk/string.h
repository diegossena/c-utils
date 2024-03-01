#pragma once
#include <sdk/types.h>
#include <sdk/error.h>

#define string_append_cstr(this, cstr) {        \
  const char ptr[] = cstr;                       \
  string_append_str(this, cstr, sizeof(cstr)); \
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

void string_constructor(string*);
void string_free(string*);

i16 string_compare(const char*, const char*);
bool string_equal(const char*, const char*);
i32 string_format(char* dest, const char* format, ...);
i32 string_format_v(char* dest, const char* format, void* va_listp);

void string_append_str(string*, const char* cstr, u64 length);
void string_append_char(string*, const char);
