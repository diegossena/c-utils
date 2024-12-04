#include <sdk/net.h>
#ifdef PLATFORM_WINDOWS

#include <winsock2.h>

export u16 net_port_from_short(u16 port) { return htons(port); }
export u16 net_port_to_short(u16 port) { return ntohs(port); }
export void _net_shutdown() { WSACleanup(); }
export error_t _net_startup() {
  WSADATA wsaData;
  error_t error = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (error) {
    _net_shutdown();
  }
  return error;
}

export u64 socket_new(task_t* task, socket_type_t type) {
  u64 this = socket(AF_INET, type, 0);
  if (this == INVALID_SOCKET) {
    return INVALID_SOCKET;
  }
  // Associate it with the I/O completion port
  if (CreateIoCompletionPort((HANDLE)this, __global_iocp, (ULONG_PTR)task, 0) == NULL) {
    // console_error("CreateIoCompletionPort", GetLastError());
    _socket_free(this);
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
export void _socket_free(u64 fd) {
  closesocket((SOCKET)fd);
}

#endif