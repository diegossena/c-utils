#ifndef SDK_PATH_H
#define SDK_PATH_H

#include <sdk/string.h>

#define PATH_MAX 260

#define path_join_cstr(this, cstr) {     \
  const char ptr[] = cstr;               \
  path_join(this, ptr, sizeof(ptr) - 1); \
}

// void path_resolve(string_t*);
// void path_join(string_t*, const char*, u64);
// void path_dirname(string_t*);

#endif