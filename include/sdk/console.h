#ifndef CONSOLE_H
#define CONSOLE_H

#include <sdk/types.h>
#include <sdk/buffer.h>
#include <sdk/string.h>
#include <sdk/error.h>
#include <sdk/stdarg.h>

#define console_write_cstr(message) { \
  const char cstr[] = message; \
  console_write_str(cstr, sizeof(cstr) - 1); \
}
#define console_log(message, ...) console_write(message "\n", ##__VA_ARGS__)

typedef enum ansi_t {
  ANSI_BOLD = 1,
  ANSI_RESET = 0,
  ANSI_UNDERLINE = 4,

  ANSI_FORE_RED = 31,
  ANSI_FORE_BLACK = 30,
  ANSI_FORE_GREEN = 32,
  ANSI_FORE_YELLOW = 33,
  ANSI_FORE_BLUE = 34,
  ANSI_FORE_MAGENTA = 35,
  ANSI_FORE_CYAN = 36,
  ANSI_FORE_WHITE = 37,
  ANSI_FORE_RESET = 39,

  ANSI_FORE_LIGHTRED = 91,
  ANSI_FORE_LIGHTBLACK = 90,
  ANSI_FORE_LIGHTGREEN = 92,
  ANSI_FORE_LIGHTYELLOW = 93,
  ANSI_FORE_LIGHTBLUE = 94,
  ANSI_FORE_LIGHTMAGENTA = 95,
  ANSI_FORE_LIGHTCYAN = 96,
  ANSI_FORE_LIGHTWHITE = 97,

  ANSI_BACK_BLUE = 44,
  ANSI_BACK_BLACK = 40,
  ANSI_BACK_RED = 41,
  ANSI_BACK_MAGENTA = 45,
  ANSI_BACK_GREEN = 42,
  ANSI_BACK_CYAN = 46,
  ANSI_BACK_YELLOW = 43,
  ANSI_BACK_WHITE = 47,
  ANSI_BACK_RESET = 49,

  ANSI_BACK_LIGHTRED = 101,
  ANSI_BACK_LIGHTBLACK = 100,
  ANSI_BACK_LIGHTGREEN = 102,
  ANSI_BACK_LIGHTYELLOW = 103,
  ANSI_BACK_LIGHTBLUE = 104,
  ANSI_BACK_LIGHTMAGENTA = 105,
  ANSI_BACK_LIGHTCYAN = 106,
  ANSI_BACK_LIGHTWHITE = 107,
} ansi_t;

export void console_color(ansi_t ansi);
export void console_write(const char* message, ...);
export void console_write_bin(u64);
export void console_write_buffer(const u8* buffer, u64 size);
export void console_write_str(const char* message, u64 length);

#endif