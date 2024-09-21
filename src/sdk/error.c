#include <sdk/error.h>

SDK_EXPORT const char* error_cstr(i32 code) {
  switch (code) {
    case ERR_OUT_OF_RANGE:
      return "ERR_OUT_OF_RANGE";
    case ERR_STREAM_READ_AFTER_END:
      return "ERR_STREAM_READ_AFTER_END";
    case ERR_STREAM_WRITE_AFTER_END:
      return "ERR_STREAM_WRITE_AFTER_END";
    case ERR_READ_NO_RESPONSE:
      return "ERR_READ_NO_RESPONSE";
    case ERR_SNMP_STAT:
      return "ERR_SNMP_STAT";
    case ERR_SNMP_STAT_ERROR:
      return "ERR_SNMP_STAT_ERROR";
    case ERR_SNMP_STAT_TIMEOUT:
      return "ERR_SNMP_STAT_TIMEOUT";
    case ERR_INVALID_URL:
      return "ERR_INVALID_URL";
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
    case ERR_NETNAME_DELETED:
      return "ERR_NETNAME_DELETED";
    case ERR_INSUFFICIENT_BUFFER:
      return "ERR_INSUFFICIENT_BUFFER";
    case ERR_WAIT_TIMEOUT:
      return "ERR_WAIT_TIMEOUT";
    case ERR_IO_PENDING:
      return "ERR_IO_PENDING";
    case ERR_NOT_FOUND:
      return "ERR_NOT_FOUND";
    case ERR_EFAULT:
      return "ERR_EFAULT";
    case ERR_EINVAL:
      return "ERR_EINVAL";
    case ERR_EWOULDBLOCK:
      return "ERR_EWOULDBLOCK";
    case ERR_EALREADY:
      return "ERR_EALREADY";
    case ERR_ENOTSOCK:
      return "ERR_ENOTSOCK";
    case ERR_EMSGSIZE:
      return "ERR_EMSGSIZE";
    case ERR_EOPNOTSUPP:
      return "ERR_EOPNOTSUPP";
    case ERR_EAFNOSUPPORT:
      return "ERR_EAFNOSUPPORT";
    case ERR_EADDRNOTAVAIL:
      return "ERR_EADDRNOTAVAIL";
    case ERR_ECONNRESET:
      return "ERR_ECONNRESET";
    case ERR_ENOBUFS:
      return "ERR_ENOBUFS";
    case ERR_ENOTCONN:
      return "ERR_ENOTCONN";
    case ERR_ETIMEDOUT:
      return "ERR_ETIMEDOUT";
    case ERR_ECONNREFUSED:
      return "ERR_ECONNREFUSED";
    case ERR_NOTINITIALISED:
      return "ERR_NOTINITIALISED";
    case ERR_CO_E_NOTINITIALIZED:
      return "ERR_CO_E_NOTINITIALIZED";
    default:
      return "ERR_UNKNOWN";
  }
}

SDK_EXPORT void error(const char* message, error_code_t code) {
  const char* error_str = error_cstr(code);
  console_color(ANSI_FORE_RED);
  console_write_cstr("Error:");
  console_color(ANSI_FORE_RESET);
  console_log(
    " %s %s",
    error_str, message
  );
}