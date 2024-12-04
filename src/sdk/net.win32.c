#include <sdk/net.h>
#ifdef PLATFORM_WINDOWS
#include <winsock2.h>

export u16 net_port_from_short(u16 port) { return htons(port); }
export u16 net_port_to_short(u16 port) { return ntohs(port); }
export void net_shutdown() { WSACleanup(); }
export error_t net_startup() {
  WSADATA wsaData;
  error_t error = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (error) {
    net_shutdown();
  }
  return error;
}

export tcp_socket_t socket_new(socket_type_t type) {
  tcp_socket_t this = socket(AF_INET, type, 0);
  u_long mode = 1;
  if (ioctlsocket(this, FIONBIO, &mode) != NO_ERROR) {
    return 0;
  }
  return this;
}
export void socket_free(u64 fd) {
  closesocket((SOCKET)fd);
}

#endif