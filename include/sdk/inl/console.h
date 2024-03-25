#pragma once

#include <sdk/console.h>
#include <sdk/string.h>
// __builtin_va_list, va_start, va_end
#include <stdarg.h>

i32 console_write(log_level_t level, const char* message, ...) {
  char out_message[80 * 25];

  __builtin_va_list args;
  va_start(args, message);
  i32 length = cstr_format_va(out_message, message, args);
  va_end(args);
  if (length < 0)
    return length;

  return console_write_str(level, out_message, length);
}