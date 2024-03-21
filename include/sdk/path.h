#pragma once

#include <sdk/string.h>

#define PATH_MAX 260

#define path_join_cstr(this, cstr) {     \
  const char ptr[] = cstr;               \
  path_join(this, ptr, sizeof(ptr) - 1); \
}

void path_resolve(string_t*);
void path_join(string_t*, const char*, u64);
void path_dirname(string_t*);

#include <sdk/path/win32.inl.h>
#include <sdk/path/inl.h>