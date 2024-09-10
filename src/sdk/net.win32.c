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
    error("WSAStartup", error_code);
    __net_shutdown();
  }
}

SDK_EXPORT u64 socket_new(task_t* task, socket_type_t type) {
  u64 this = WSASocketW(AF_INET, type, 0, 0, 0, WSA_FLAG_OVERLAPPED);
  if (this == INVALID_SOCKET) {
    error("socket", WSAGetLastError());
    return this;
  }
  if (this > __net_max_fd) {
    __net_max_fd = this + 1;
  }
  /**
   * If non_blocking = 0, blocking is enabled;
   * If non_blocking != 0, non-blocking mode is enabled.
   */
  u_long non_blocking = 1;
  if (ioctlsocket(this, FIONBIO, &non_blocking) == SOCKET_ERROR) {
    error("ioctlsocket", WSAGetLastError());
    return this;
  }
  // Associate it with the I/O completion port
  if (CreateIoCompletionPort((HANDLE)this, task->taskmanager->iocp, (ULONG_PTR)task, 0) == NULL) {
    error("CreateIoCompletionPort", GetLastError());
  }
  return this;
}
SDK_EXPORT void _socket_free(u64 fd) { closesocket((SOCKET)fd); }

#endif