#include <sdk/console.h>

// __builtin_va_list, va_start, va_end
#include <stdarg.h>

export void console_write(const char* message, ...) {
  __builtin_va_list args;
  va_start(args, message);
  char buffer[BUFFER_DEFAULT_SIZE];
  i32 length = string_format_va(buffer, BUFFER_DEFAULT_SIZE, message, args);
  va_end(args);
  if (length < 0)
    return;
  console_write_str(buffer, length);
}
export void console_write_bin(u64 value) {
  char str[76] = {};
  char* ptr = str;
  if (value) {
    u8 place = 63;
    while (true) {
      if ((1LLU << place) <= value) {
        place += 3 - place % 4;
        break;
      }
      --place;
    }
    while (true) {
      *ptr++ = value & (1 << place) ? '1' : '0';
      if (place == 0)
        break;
      if (place % 4 == 0) {
        *ptr++ = ' ';
      }
      --place;
    }
  } else {
    str[0] = str[1] = str[2] = str[3] = '0';
    ptr += 4;
  }
  console_write_str(str, ptr - str);
}
export void console_write_buffer(const u8* buffer, u64 size) {
  if (size) {
    console_write("%02x", *buffer);
    ++buffer;
    if (--size) {
      while (true) {
        console_write(" %02x", *buffer);
        if (!--size)
          break;
        ++buffer;
      }
    }
  }
}