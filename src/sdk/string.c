#include <sdk/string.h>

/* vsnprintf */
#include <stdio.h>

SDK_EXPORT i32 cstr_format_va(char* stream, u64 size, const char* format, void* args) {
  i32 length = vsnprintf(stream, size, format, args);
  if (length < 0) {
    error("vsnprintf", length);
  }
  return length;
}