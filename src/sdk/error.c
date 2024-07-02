#include <sdk/error.h>

SDK_EXPORT const char* error_cstr(i32 code) {
  switch (code) {
    case ERR_OUT_OF_RANGE:
      return "ERR_OUT_OF_RANGE";
    // Stream
    case ERR_STREAM_READ_AFTER_END:
      return "ERR_STREAM_READ_AFTER_END";
    case ERR_STREAM_WRITE_AFTER_END:
      return "ERR_STREAM_WRITE_AFTER_END";
    case ERR_READ_NO_RESPONSE:
      return "ERR_READ_NO_RESPONSE";
    // SNMP
    case ERR_SNMP_STAT:
      return "ERR_SNMP_STAT";
    case ERR_SNMP_STAT_ERROR:
      return "ERR_SNMP_STAT_ERROR";
    case ERR_SNMP_STAT_TIMEOUT:
      return "ERR_SNMP_STAT_TIMEOUT";
    // URL
    case ERR_INVALID_URL:
      return "ERR_INVALID_URL";
    // system
    case ERR_SUCCESS:
      return "ERR_SUCCESS";
    case ERR_INVALID_FUNCTION:
      return "ERR_INVALID_FUNCTION";
    case ERR_FILE_NOT_FOUND:
      return "ERR_FILE_NOT_FOUND";
    case ERR_PATH_NOT_FOUND:
      return "ERR_PATH_NOT_FOUND";
    case ERR_TOO_MANY_OPEN_FILES:
      return "ERR_TOO_MANY_OPEN_FILES";
    case ERR_ACCESS_DENIED:
      return "ERR_ACCESS_DENIED";
    case ERR_INVALID_HANDLE:
      return "ERR_INVALID_HANDLE";
    case ERR_ARENA_TRASHED:
      return "ERR_ARENA_TRASHED";
    case ERR_NOT_ENOUGH_MEMORY:
      return "ERR_NOT_ENOUGH_MEMORY";
    case ERR_INVALID_BLOCK:
      return "ERR_INVALID_BLOCK";
    case ERR_BAD_ENVIRONMENT:
      return "ERR_BAD_ENVIRONMENT";
    case ERR_BAD_FORMAT:
      return "ERR_BAD_FORMAT";
    case ERR_INSUFFICIENT_BUFFER:
      return "ERR_INSUFFICIENT_BUFFER";
    // winsock2
    case ERR_WSAEFAULT:
      return "ERR_WSAEFAULT";
    case ERR_WSAEINVAL:
      return "ERR_WSAEINVAL";
    case ERR_WSAEWOULDBLOCK:
      return "ERR_WSAEWOULDBLOCK";
    case ERR_WSAENOTSOCK:
      return "ERR_WSAENOTSOCK";
    case ERR_WSAEMSGSIZE:
      return "ERR_WSAEMSGSIZE";
    case ERR_WSAEOPNOTSUPP:
      return "ERR_WSAEOPNOTSUPP";
    case ERR_WSAEAFNOSUPPORT:
      return "ERR_WSAEAFNOSUPPORT";
    case ERR_EADDRNOTAVAIL:
      return "ERR_EADDRNOTAVAIL";
    case ERR_WSAECONNRESET:
      return "ERR_WSAECONNRESET";
    case ERR_WSAENOBUFS:
      return "ERR_WSAENOBUFS";
    case ERR_ETIMEDOUT:
      return "ERR_ETIMEDOUT";
    case ERR_WSAECONNREFUSED:
      return "ERR_WSAECONNREFUSED";
    case ERR_WSANOTINITIALISED:
      return "ERR_WSANOTINITIALISED";
    case ERR_CO_E_NOTINITIALIZED:
      return "ERR_CO_E_NOTINITIALIZED";
    default:
      return "ERR_UNKNOWN";
  }
}

void error(const char* message, error_code_t code) {
  const char* error_str = error_cstr(code);
  console_color(ANSI_FORE_RED);
  console_write_cstr("Error:");
  console_color(ANSI_FORE_RESET);
  console_log(
    " %s %x %s",
    error_str, code, message
  );
}