#include "internal/platform.h"

#if PLATFORM_WINDOWS

#include "internal/net.tcp.h"
#include <winsock2.h>

#include "sdk/console.h" // TODO: remove this line

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
void net_tcp_connect_handle(net_tcp_t* this) {
  static struct timeval timeout = {};
  fd_set readable, writable;
  FD_SET(this->socket, &readable);
  FD_SET(this->socket, &writable);
  error_last = select(app_global.max_fd + 1, &readable, &writable, null, &timeout);
  if (error_last == -1) {
    error("select", ERR_ETIMEDOUT);
    this->stream.task.type = TASK_NONE;
  } else if (error_last > 0) {
    this->stream.connection_cb(&this->stream);
    this->stream.task.type = TASK_NONE;
  }
}

#endif