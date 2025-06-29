#include <sdk/error.h>

const char* error_cstr(error_t code) {
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
    case ERR_SEM_TIMEOUT:
      return "ERR_SEM_TIMEOUT";
    case ERR_INSUFFICIENT_BUFFER:
      return "ERR_INSUFFICIENT_BUFFER";
    case ERR_WAIT_TIMEOUT:
      return "ERR_WAIT_TIMEOUT";
    case ERR_OPERATION_ABORTED:
      return "ERR_OPERATION_ABORTED";
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
    case ERR_EADDRINUSE:
      return "ERR_EADDRINUSE";
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
    case ERR_WIC_FILE_NOT_FOUND:
      return "ERR_WIC_FILE_NOT_FOUND";
    default:
      return "ERR_UNKNOWN";
  }
}
void error(error_t code, const char* message, ...) {
  i32 length;
  char buffer[TEXT_SIZE];
  char* buffer_end = buffer + sizeof(buffer) - 1;
  length = string_format(buffer, sizeof(buffer), "Error: %s %x ", error_cstr(code), code);
  if (length < 0)
    return;
  char* ptr = buffer + length;
  va_list args;
  va_start(args, message);
  length = string_format_va(ptr, buffer_end - ptr, message, args);
  va_end(args);
  if (length < 0)
    return;
  ptr += length;
  *ptr++ = '\n';
  console_write_str(buffer, ptr - buffer);
}