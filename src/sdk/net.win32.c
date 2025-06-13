#include <sdk/net.h>
#ifdef PLATFORM_WINDOWS
#include <winsock2.h>

net_port_t net_port_from_short(u16 port) { return htons(port); }
u16 net_port_to_short(net_port_t port) { return ntohs(port); }
error_t net_startup() {
  WSADATA wsaData;
  error_t error = WSAStartup(MAKEWORD(2, 2), &wsaData);
#ifdef DEBUG
  __net_started = true;
#endif
  if (error) {
    net_shutdown();
  }
  return error;
}
void net_shutdown() {
  WSACleanup();
#ifdef DEBUG
  __net_started = false;
#endif
}
error_t net_error() {
  return WSAGetLastError();
}
socket_t _socket_new(socket_type_t type) {
  socket_t this = socket(AF_INET, type, 0);
  if (this == INVALID_SOCKET) {
    return 0;
  }
  __socket_count_increment();
  return this;
}
void _socket_free(socket_t fd) {
  closesocket((SOCKET)fd);
  __socket_count_decrement();
}

#endif