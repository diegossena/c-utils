#ifndef SDK_ERROR_H
#define SDK_ERROR_H

#include <sdk/types.h>

#include <sdk/console.h>

typedef enum error_code_t {
  ERR_MIN = MIN_I16,
  ERR_INVALID_URL,
  ERR_STREAM_WRITE_AFTER_END,
  ERR_STREAM_READ_AFTER_END,
  ERR_READ_NO_RESPONSE,
  ERR_OUT_OF_RANGE,
  // snmp
  ERR_SNMP_STAT,
  ERR_SNMP_STAT_ERROR,
  ERR_SNMP_STAT_TIMEOUT,
  // http
  ERR_HTTP,
  ERR_UNSUPPORTED_PROTOCOL,
  ERR_FAILED_INIT,
  ERR_URL_MALFORMAT,
  ERR_NOT_BUILT_IN,
  ERR_COULDNT_RESOLVE_PROXY,
  ERR_ENOTFOUND,
  ERR_UNKNOWN = -1,
  // https://learn.microsoft.com/en-us/windows/win32/debug/system-error-codes--0-499-
  ERR_SUCCESS = 0,
  ERR_INVALID_FUNCTION = 1,
  ERR_FILE_NOT_FOUND = 2,
  ERR_PATH_NOT_FOUND = 3,
  ERR_TOO_MANY_OPEN_FILES = 4,
  ERR_ACCESS_DENIED = 5,
  ERR_INVALID_HANDLE = 6,
  ERR_ARENA_TRASHED = 7,
  ERR_NOT_ENOUGH_MEMORY = 8,
  ERR_INVALID_BLOCK = 9,
  ERR_BAD_ENVIRONMENT = 10,
  ERR_BAD_FORMAT = 11,
  ERR_NETNAME_DELETED = 64,
  ERR_SEM_TIMEOUT = 121,
  ERR_INSUFFICIENT_BUFFER = 122,
  ERR_WAIT_TIMEOUT = 258,
  // https://learn.microsoft.com/en-us/windows/win32/debug/system-error-codes--500-999-
  ERR_OPERATION_ABORTED = 995,
  ERR_IO_PENDING = 997,
  // https://learn.microsoft.com/en-us/windows/win32/debug/system-error-codes--1000-1299-
  ERR_NOT_FOUND = 1168,
  // https://learn.microsoft.com/pt-br/windows/win32/winsock/windows-sockets-error-codes-2
  ERR_EFAULT = 10014,
  ERR_EINVAL = 10022,
  ERR_EWOULDBLOCK = 10035,
  ERR_EALREADY = 10037,
  ERR_ENOTSOCK = 10038,
  ERR_EMSGSIZE = 10040,
  ERR_EOPNOTSUPP = 10045,
  ERR_EAFNOSUPPORT = 10047,
  ERR_EADDRNOTAVAIL = 10049,
  ERR_ECONNRESET = 10054,
  ERR_ENOBUFS = 10055,
  ERR_ENOTCONN = 10057,
  ERR_ETIMEDOUT = 10060,
  ERR_ECONNREFUSED = 10061,
  ERR_NOTINITIALISED = 10093,
  ERR_CO_E_NOTINITIALIZED = 0x800401f0
} error_code_t;

SDK_EXPORT const char* error_cstr(i32 code);
SDK_EXPORT void error(const char* message, error_code_t code);

#endif