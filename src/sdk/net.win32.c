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
SDK_EXPORT u64 socket_new(socket_type_t type) {
  u64 this = socket(AF_INET, type, 0);
  if (this == INVALID_SOCKET) {
    error("socket", WSAGetLastError());
    return this;
  }
  /**
   * If iMode = 0, blocking is enabled;
   * If iMode != 0, non-blocking mode is enabled.
   */
  u_long mode = 1;
  if (ioctlsocket(this, FIONBIO, &mode) == SOCKET_ERROR) {
    error("ioctlsocket", WSAGetLastError());
    return this;
  }
  if (this > __net_max_fd) {
    __net_max_fd = this + 1;
  }
  return this;
}
SDK_EXPORT void _socket_free(u64 fd) { closesocket((SOCKET)fd); }

#endif