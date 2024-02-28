#pragma once
#include <sdk/types.h>

typedef struct string {
  const char* buffer;
  u64 length;
} string;

string string_cstr(const char*);
u64 string_length(const char*);
char* string_copy(const char*);
void string_free();
