#include <sdk/net.h>
#ifdef PLATFORM_WINDOWS

#include <winsock2.h>

SDK_EXPORT u16 net_port_from_short(u16 port) { return htons(port); }
SDK_EXPORT u16 net_port_to_short(u16 port) { return ntohs(port); }
SDK_EXPORT void __net_shutdown() { WSACleanup(); }
SDK_EXPORT void __net_startup() {
  WSADATA wsaData;
  i32 error_code = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (error_code) {
    error_log("WSAStartup", error_code);
    __net_shutdown();
  }
}

SDK_EXPORT u64 socket_new(task_t* task, socket_type_t type) {
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
SDK_EXPORT void _socket_cancel(u64 fd) {
  i32 result = CancelIoEx((HANDLE)fd, 0);
  if (result == 0) {
    result = GetLastError();
    if (result != ERR_NOT_FOUND) {
      error_log("CancelIoEx", GetLastError());
    }
  }
}
SDK_EXPORT void _socket_free(u64 fd) {
  closesocket((SOCKET)fd);
}

#endif