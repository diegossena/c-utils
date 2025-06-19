#include <sdk/console.h>

void console_write(const char* message, ...) {
  va_list args;
  va_start(args, message);
  char buffer[TEXT_SIZE];
  i32 length = string_format_va(buffer, sizeof(buffer) + 1, message, args);
  va_end(args);
  if (length < 0)
    return;
  console_write_str(buffer, length);
}
void console_write_bin(u64 value) {
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
void console_write_buffer(const u8* data, u64 size) {
  bool first = true;
  char buffer[7] = ", 0x";
  char* hex_ptr = buffer + 4;
  while (size) {
    string_format(hex_ptr, 3, "%02x", *data++);
    if (first) {
      first = false;
      console_write_str(buffer + 2, 4);
    } else {
      console_write_str(buffer, 6);
    }
    --size;
  }
}