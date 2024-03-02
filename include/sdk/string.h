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
class string string;

string* string_new();
void string_free(string*);

const char* string_cstr(const string*);
u64 string_length(const string*);

void string_append_string(string*, string*);
void string_append_str(string* this, const char* cstr, u64 length);
void string_append_char(string* this, const char);
bool string_equal_cstr(const string*, const char*);
bool string_equal_string(const string*, const string*);
i16 string_compare(const string*, const string*);

i16 cstr_compare(const char*, const char*);
bool cstr_equal(const char*, const char*);
i32 cstr_format(char*, const char* format, ...);
i32 cstr_format_v(char* this, const char* format, void* va_listp);
u64 cstr_length(const char*);