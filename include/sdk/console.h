#pragma once

#include <sdk/types.h>
#include <sdk/error.h>

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

#define console_fatal(message, ...) _console_write(LOG_LEVEL_FATAL, message "\n", ##__VA_ARGS__)
#define console_warn(message, ...) _console_write(LOG_LEVEL_WARN, message "\n", ##__VA_ARGS__)
#define console_info(message, ...) _console_write(LOG_LEVEL_INFO, message "\n", ##__VA_ARGS__)
#define console_debug(message, ...) _console_write(LOG_LEVEL_DEBUG, message "\n", ##__VA_ARGS__)
#define console_trace(message, ...) _console_write(LOG_LEVEL_TRACE, message "\n", ##__VA_ARGS__)

#define console_log(message, ...) _console_write(LOG_LEVEL_INFO, message "\n", ##__VA_ARGS__)
#define console_write(message, ...) _console_write(LOG_LEVEL_INFO, message, ...)

#define console_error(message, ...) _console_write(LOG_LEVEL_ERROR, message "\n", ##__VA_ARGS__)
#define console_error_write(message, ...) _console_write(LOG_LEVEL_ERROR, message, ##__VA_ARGS__)

error_code console_inicialize();

error_code _console_write(log_level, const char* message, ...);

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