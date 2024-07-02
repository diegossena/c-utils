#include <sdk/console.h>

// __builtin_va_list, va_start, va_end
#include <stdarg.h>

SDK_EXPORT void console_write(const char* message, ...) {
  __builtin_va_list args;
  va_start(args, message);
  i32 length = cstr_format_va(buffer_shared, BUFFER_SIZE, message, args);
  va_end(args);
  if (length < 0)
    return;
  console_write_str(buffer_shared, length);
}
SDK_EXPORT void console_write_buffer(const u8* buffer, u64 size) {
  bool first = true;
  console_write_cstr("<Buffer ");
  if (size) {
    console_write("%02x", *buffer);
    if (--size) {
      while (true) {
        console_write(" %02x", *buffer);
        if (!--size)
          break;
        ++buffer;
      }
    }
  }
  console_write_cstr(">");
}