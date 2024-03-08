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
typedef struct string_t string_t;

string_t* string_new();
void string_free(string_t*);

const char* string_cstr(const string_t*);
u64 string_length(const string_t*);

void string_append_string(string_t*, string_t*);
void string_append_str(string_t* this, const char* cstr, u64 length);
void string_append_char(string_t* this, const char);
bool string_equal_cstr(const string_t*, const char*);
bool string_equal_string(const string_t*, const string_t*);
i16 string_compare(const string_t*, const string_t*);

i16 cstr_compare(const char*, const char*);
bool cstr_equal(const char*, const char*);
i32 cstr_format_v(char* this, const char* format, void* va_list_p);
i32 cstr_format(char*, const char* format, ...);
u64 cstr_length(const char*);
