#include "base/string.h"

u64 cstr_length(const char* cstr) {
  const char* begin = cstr;
  while (*cstr) ++cstr;
  return cstr - begin;
}