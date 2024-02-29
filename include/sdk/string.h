#pragma once
#include <sdk/types.h>

/*
```c
string str = {}

string str;
string_new(str);
```
*/
typedef struct string {
  char* data;
  u64 length;
} string;

void string_new(string*);
void string_free(string*);

i16 string_compare(const char*, const char*);
bool string_equal(const char*, const char*);

void string_append_cstr(string*, const char*);
void string_append_char(string*, const char);
