#include "sdk/console.h"
#include "sdk/string.h" // string_format

#include "base/console.h" // base_console_write
#include "base/memory.h" // MAX_BUFSIZ

#include <stdarg.h> // __builtin_va_list, va_start, va_end

#define ANSI(A) "\033[" #A "m"

error_code _console_write(log_level level, const char* message, ...) {
  char out_message[MAX_BUFSIZ];

  __builtin_va_list args;
  va_start(args, message);
  i32 length = string_format_v(out_message, message, args);
  va_end(args);
  if (length < 0)
    return length;

  return base_console_write(level, out_message, length);
}

const char* console_reset = ANSI(0);
const char* console_bold = ANSI(1);
const char* console_underline = ANSI(4);

const char* console_fore_black = ANSI(30);
const char* console_fore_red = ANSI(31);
const char* console_fore_green = ANSI(32);
const char* console_fore_yellow = ANSI(33);
const char* console_fore_blue = ANSI(34);
const char* console_fore_magenta = ANSI(35);
const char* console_fore_cyan = ANSI(36);
const char* console_fore_white = ANSI(37);
const char* console_fore_reset = ANSI(39);

const char* console_fore_lightblack = ANSI(90);
const char* console_fore_lightred = ANSI(91);
const char* console_fore_lightgreen = ANSI(92);
const char* console_fore_lightyellow = ANSI(93);
const char* console_fore_lightblue = ANSI(94);
const char* console_fore_lightmagenta = ANSI(95);
const char* console_fore_lightcyan = ANSI(96);
const char* console_fore_lightwhite = ANSI(97);

const char* console_back_black = ANSI(40);
const char* console_back_blue = ANSI(44);
const char* console_back_red = ANSI(41);
const char* console_back_magenta = ANSI(45);
const char* console_back_green = ANSI(42);
const char* console_back_cyan = ANSI(46);
const char* console_back_yellow = ANSI(43);
const char* console_back_white = ANSI(47);
const char* console_back_reset = ANSI(49);

const char* console_back_lightblack = ANSI(100);
const char* console_back_lightred = ANSI(101);
const char* console_back_lightgreen = ANSI(102);
const char* console_back_lightyellow = ANSI(103);
const char* console_back_lightblue = ANSI(104);
const char* console_back_lightmagenta = ANSI(105);
const char* console_back_lightcyan = ANSI(106);
const char* console_back_lightwhite = ANSI(107);