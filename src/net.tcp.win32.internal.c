#include "internal/platform.h"

#if PLATFORM_WINDOWS

// app_global
#include "internal/application.h"
// net_tcp_t
#include "internal/net.tcp.h"
// TODO: remove this line
#include "sdk/console.h" 
#include <winsock2.h>

struct timeval timeout = {};

void net_tcp_listen_handle(net_tcp_t* this) {
  SOCKET client_socket = accept(this->stream.fd, 0, 0);
  if (client_socket == INVALID_SOCKET) {
    error_last = WSAGetLastError();
    if (error_last != WSAEWOULDBLOCK) {
      error("accept", error_last);
    }
  } else if (this->stream.task.handle) {
    this->stream.task.handle(&this->stream);
  }
}
void net_tcp_connect_handle(net_tcp_t* this) {
  fd_set readable, writable;
  FD_SET(this->stream.fd, &readable);
  FD_SET(this->stream.fd, &writable);
  error_last = select(app_global.max_fd, &readable, &writable, null, &timeout);
  if (error_last > 0) {
    if (this->stream.task.handle) {
      this->stream.task.handle(&this->stream);
    }
    this->stream.task.type = TASK_NONE;
  }
}

#endif