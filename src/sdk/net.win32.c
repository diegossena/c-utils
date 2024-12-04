#include <sdk/net.h>
#ifdef PLATFORM_WINDOWS

#include <winsock2.h>

export u16 net_port_from_short(u16 port) { return htons(port); }
export u16 net_port_to_short(u16 port) { return ntohs(port); }
export void __net_shutdown() { WSACleanup(); }
export void __net_startup() {
  WSADATA wsaData;
  i32 error_code = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (error_code) {
    error_log("WSAStartup", error_code);
    __net_shutdown();
  }
}

export u64 socket_new(task_t* task, socket_type_t type) {
  u64 this = socket(AF_INET, type, 0);
  if (this == INVALID_SOCKET) {
    error_log("socket", WSAGetLastError());
    return this;
  }
  // Associate it with the I/O completion port
  if (CreateIoCompletionPort((HANDLE)this, __global_iocp, (ULONG_PTR)task, 0) == NULL) {
    error_log("CreateIoCompletionPort", GetLastError());
  }
  return this;
}
export void _socket_cancel(u64 fd) {
  i32 result = CancelIoEx((HANDLE)fd, 0);
  if (result == 0) {
    result = GetLastError();
    if (result != ERR_NOT_FOUND) {
      error_log("CancelIoEx", GetLastError());
    }
  }
}
export void _socket_free(u64 fd) {
  closesocket((SOCKET)fd);
}

#endif