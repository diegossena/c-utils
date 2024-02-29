#include "sdk/error.h"
#include "sdk/console.h"

#include <string.h> // strerror

const char* error_cstr(i32 code) {
  switch (code) {
    case ERR_OUT_OF_RANGE: return "ERR_OUT_OF_RANGE";
    // Stream
    case ERR_STREAM_READ_AFTER_END: return "ERR_STREAM_READ_AFTER_END";
    case ERR_STREAM_WRITE_AFTER_END: return "ERR_STREAM_WRITE_AFTER_END";
    case ERR_READ_NO_RESPONSE: return "ERR_READ_NO_RESPONSE";
    // SNMP
    case ERR_SNMP_STAT: return "ERR_SNMP_STAT";
    case ERR_SNMP_STAT_ERROR: return "ERR_SNMP_STAT_ERROR";
    case ERR_SNMP_STAT_TIMEOUT: return "ERR_SNMP_STAT_TIMEOUT";
    // URL
    case ERR_INVALID_URL: return "ERR_INVALID_URL";
    // Others
    case ERR_UNKNOWN: return "ERR_UNKNOWN";
    default:
      return strerror(code);
  }
}


void error(const char* message, error_code code) {
  const char* error_str = error_cstr(code);
  console_error("%sError:%s %s %s", console_fore_red, console_reset, error_str, message);
}