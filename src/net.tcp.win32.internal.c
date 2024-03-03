#include "internal/platform.h"

#if PLATFORM_WINDOWS

#include "internal/net.tcp.h"
#include <winsock2.h>

#include <stdio.h>

void net_tcp_listen_handle(net_tcp_t* this) {
  SOCKET client_socket = accept(this->socket, 0, 0);
  if (client_socket == INVALID_SOCKET) {
    error_last = WSAGetLastError();
    if (error_last != WSAEWOULDBLOCK) {
      error("accept", error_last);
    }
  } else if (this->stream.connection_cb) {
    this->stream.connection_cb(&this->stream);
  }
}

#endif