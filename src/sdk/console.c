#include <sdk/console.h>

export void console_write(const char* message, ...) {
  va_list args;
  va_start(args, message);
  char buffer[TEXT_SIZE + 1];
  i32 length = string_format_va(buffer, sizeof(buffer), message, args);
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
export void console_write_buffer(const char* data, u64 size) {
  if (size) {
    char hex_str[TEXT_SIZE + 1];
    char* ptr = hex_str;
    string_format(ptr, 3, "%02x", *data++);
    while (size--) {
      ptr += 3;
      string_format(ptr, 4, " %02x", *data++);
    }
    console_write_str(hex_str, ptr - hex_str);
  }
}