#pragma once

#include <sdk/types.h>
#include <sdk/error.h>

#define console_write_cstr(level, message, length) { \
  const char cstr[] = message; \
  console_write_str(level, message, length); \
}
#define console_fatal(message, ...) _console_write(LOG_LEVEL_FATAL, message "\n", ##__VA_ARGS__)
#define console_warn(message, ...) _console_write(LOG_LEVEL_WARN, message "\n", ##__VA_ARGS__)
#define console_info(message, ...) _console_write(LOG_LEVEL_INFO, message "\n", ##__VA_ARGS__)
#define console_debug(message, ...) _console_write(LOG_LEVEL_DEBUG, message "\n", ##__VA_ARGS__)
#define console_trace(message, ...) _console_write(LOG_LEVEL_TRACE, message "\n", ##__VA_ARGS__)
#define console_log(message, ...) _console_write(LOG_LEVEL_INFO, message "\n", ##__VA_ARGS__)
#define console_log_str(message, length) { \
  console_write_str(LOG_LEVEL_INFO, message, length); \
}
#define console_log_cstr(message) { \
  const char cstr[] = message "\n"; \
  console_write_str(LOG_LEVEL_INFO, cstr, sizeof(cstr) - 1); \
}
#define console_write(message, ...) _console_write(LOG_LEVEL_INFO, message, ...)
#define console_error(message, ...) _console_write(LOG_LEVEL_ERROR, message "\n", ##__VA_ARGS__)
#define console_error_cstr(message) { \
  const char cstr[] = message "\n"; \
  console_write_str(LOG_LEVEL_ERROR, cstr, sizeof(cstr) - 1); \
}
#define console_error_write(message, ...) _console_write(LOG_LEVEL_ERROR, message, ##__VA_ARGS__)

#define ANSI(A) "\033[" #A "m"

#define CONSOLE_RESET ANSI(0)
#define CONSOLE_BOLD ANSI(1)
#define CONSOLE_UNDERLINE ANSI(4)

#define CONSOLE_FORE_BLACK ANSI(30)
#define CONSOLE_FORE_RED ANSI(31)
#define CONSOLE_FORE_GREEN ANSI(32)
#define CONSOLE_FORE_YELLOW ANSI(33)
#define CONSOLE_FORE_BLUE ANSI(34)
#define CONSOLE_FORE_MAGENTA ANSI(35)
#define CONSOLE_FORE_CYAN ANSI(36)
#define CONSOLE_FORE_WHITE ANSI(37)
#define CONSOLE_FORE_RESET ANSI(39)

#define CONSOLE_FORE_LIGHTBLACK ANSI(90)
#define CONSOLE_FORE_LIGHTRED ANSI(91)
#define CONSOLE_FORE_LIGHTGREEN ANSI(92)
#define CONSOLE_FORE_LIGHTYELLOW ANSI(93)
#define CONSOLE_FORE_LIGHTBLUE ANSI(94)
#define CONSOLE_FORE_LIGHTMAGENTA ANSI(95)
#define CONSOLE_FORE_LIGHTCYAN ANSI(96)
#define CONSOLE_FORE_LIGHTWHITE ANSI(97)

#define CONSOLE_BACK_BLACK ANSI(40)
#define CONSOLE_BACK_BLUE ANSI(44)
#define CONSOLE_BACK_RED ANSI(41)
#define CONSOLE_BACK_MAGENTA ANSI(45)
#define CONSOLE_BACK_GREEN ANSI(42)
#define CONSOLE_BACK_CYAN ANSI(46)
#define CONSOLE_BACK_YELLOW ANSI(43)
#define CONSOLE_BACK_WHITE ANSI(47)
#define CONSOLE_BACK_RESET ANSI(49)

#define CONSOLE_BACK_LIGHTBLACK ANSI(100)
#define CONSOLE_BACK_LIGHTRED ANSI(101)
#define CONSOLE_BACK_LIGHTGREEN ANSI(102)
#define CONSOLE_BACK_LIGHTYELLOW ANSI(103)
#define CONSOLE_BACK_LIGHTBLUE ANSI(104)
#define CONSOLE_BACK_LIGHTMAGENTA ANSI(105)
#define CONSOLE_BACK_LIGHTCYAN ANSI(106)
#define CONSOLE_BACK_LIGHTWHITE ANSI(107)

typedef enum log_level {
  // @brief Fatal log level, should be used to stop the application when hit.
  LOG_LEVEL_FATAL,
  // @brief Error log level, should be used to indicate critical runtime problems that cause the application to run improperly or not at all.
  LOG_LEVEL_ERROR,
  // Warning log level, should be used to indicate non-critial problems with the application that cause it to run suboptimally
  LOG_LEVEL_WARN,
  // @brief Info log level, should be used for non-erronuous informational purposes.
  LOG_LEVEL_INFO,
  // @brief Debug log level, should be used for debugging purposes.
  LOG_LEVEL_DEBUG,
  // @brief Trace log level, should be used for verbose debugging purposes.
  LOG_LEVEL_TRACE
} log_level;

extern const char* console_reset;
extern const char* console_bold;
extern const char* console_underline;

extern const char* console_fore_black;
extern const char* console_fore_red;
extern const char* console_fore_green;
extern const char* console_fore_yellow;
extern const char* console_fore_blue;
extern const char* console_fore_magenta;
extern const char* console_fore_cyan;
extern const char* console_fore_white;
extern const char* console_fore_reset;

extern const char* console_fore_lightblack;
extern const char* console_fore_lightred;
extern const char* console_fore_lightgreen;
extern const char* console_fore_lightyellow;
extern const char* console_fore_lightblue;
extern const char* console_fore_lightmagenta;
extern const char* console_fore_lightcyan;
extern const char* console_fore_lightwhite;

extern const char* console_back_black;
extern const char* console_back_blue;
extern const char* console_back_red;
extern const char* console_back_magenta;
extern const char* console_back_green;
extern const char* console_back_cyan;
extern const char* console_back_yellow;
extern const char* console_back_white;
extern const char* console_back_reset;

extern const char* console_back_lightblack;
extern const char* console_back_lightred;
extern const char* console_back_lightgreen;
extern const char* console_back_lightyellow;
extern const char* console_back_lightblue;
extern const char* console_back_lightmagenta;
extern const char* console_back_lightcyan;
extern const char* console_back_lightwhite;

error_code console_inicialize();

error_code _console_write(log_level, const char* message, ...);
error_code console_write_str(log_level, const char* message, u64 length);