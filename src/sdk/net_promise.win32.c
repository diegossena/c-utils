#include <sdk/net_promise.h>
#ifdef PLATFORM_WINDOWS
#include <winsock2.h>

export u64 socket_promise_new(task_t* task, socket_type_t type) {
  u64 this = socket(AF_INET, type, 0);
  if (this == INVALID_SOCKET) {
    return INVALID_SOCKET;
  }
  // Associate it with the I/O completion port
  if (CreateIoCompletionPort((HANDLE)this, __global_iocp, (ULONG_PTR)task, 0) == NULL) {
    // console_error("CreateIoCompletionPort", GetLastError());
    socket_free(this);
    return INVALID_SOCKET;
  }
  return this;
}
export void _socket_cancel(u64 fd) {
  err_t result = CancelIoEx((HANDLE)fd, 0);
  if (result == 0) {
    result = GetLastError();
    if (result != ERR_NOT_FOUND) {
      // console_error("CancelIoEx", GetLastError());
    }
  }
}

#endif