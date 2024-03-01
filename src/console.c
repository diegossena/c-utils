#include "sdk/console.h"
#include "sdk/string.h" // string_format

#include "base/console.h" // base_console_write
#include "base/memory.h" // MAX_BUFSIZ

#include <stdarg.h> // __builtin_va_list, va_start, va_end

error_code _console_write(log_level level, const char* message, ...) {
  char out_message[MAX_BUFSIZ];

  __builtin_va_list args;
  va_start(args, message);
  i32 length = string_format_v(out_message, message, args);
  va_end(args);
  if (length < 0)
    return length;

  return console_write_str(level, out_message, length);
}