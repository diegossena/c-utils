#include <sdk/string.h>

/* vsnprintf */
#include <stdio.h>

SDK_EXPORT u64 string_length(const char* str) {
  const char* ptr = str;
  while (*ptr != '\0') ++ptr;
  return ptr - str;
}
SDK_EXPORT i32 cstr_format_va(char* stream, u64 size, const char* format, void* args) {
  i32 length = vsnprintf(stream, size, format, args);
  if (length < 0) {
    error_log("vsnprintf", length);
  }
  return length;
}
SDK_EXPORT i32 cstr_format(char* stream, const char* format, ...) {
  __builtin_va_list argv;
  __builtin_va_start(argv, format);
  i32 length = vsprintf(stream, format, argv);
  if (length < 0) {
    error_log("vsprintf", length);
  }
  return length;
}