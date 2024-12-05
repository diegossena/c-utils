#include <sdk/net.h>
#ifdef PLATFORM_WINDOWS
#include <winsock2.h>

export net_port_t net_port_from_short(u16 port) { return htons(port); }
export u16 net_port_to_short(net_port_t port) { return ntohs(port); }
export void net_shutdown() { WSACleanup(); }
export error_t net_startup() {
  WSADATA wsaData;
  error_t error = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (error) {
    net_shutdown();
  }
  return error;
}

export socket_t _socket_new(socket_type_t type) {
  return socket(AF_INET, type, 0);
}
export void _socket_free(socket_t fd) {
  closesocket((SOCKET)fd);
}

#endif