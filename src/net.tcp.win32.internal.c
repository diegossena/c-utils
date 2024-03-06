#include "internal/platform.h"

#if PLATFORM_WINDOWS

// app_global
#include "internal/application.h"
// net_tcp_t
#include "internal/net.tcp.h"
// memory_free
#include "internal/memory.h"
// TODO: remove this line
#include "sdk/console.h"
// date_now
#include "sdk/date.h"
// closesocket
#include <winsock2.h>

void net_tcp_close_handle(net_tcp_t* this) {
  shutdown(this->socket, FD_WRITE | FD_READ);
  queue_remove(&app_global.tasks, &this->task.queue);
  closesocket(this->socket);
  memory_free(this);
}

void net_tcp_connect_handle(net_tcp_t* this) {
  struct timeval timeout = {};
  fd_set writable = {};
  FD_SET(this->socket, &writable);
  error_last = select(app_global.max_fd, null, &writable, null, &timeout);
  if (error_last > 0) {
    // connected
    this->handle(this);
  } else if (error_last == SOCKET_ERROR) {
    error_last = WSAGetLastError();
    if (error_last != WSAEWOULDBLOCK) {
      error("select", error_last);
      net_tcp_close_handle(this);
      return;
    }
    // connecting...
  }
}
void net_tcp_write_handle(net_tcp_t* this) {
  u64 remaining = this->stream.length - this->stream.processed;
  const byte* buffer_start = this->stream.writable + this->stream.processed;
  i32 sent = send(this->socket, buffer_start, remaining, 0);
  if (sent >= 0) {
    this->stream.processed += sent;
    if (this->stream.length == this->stream.processed) {
      memory_free(this->stream.writable);
      this->task.type = TASK_TCP_CLOSING;
      this->handle(this, this->stream.context);
      if (this->task.type == TASK_TCP_CLOSING) {
        net_tcp_close_handle(this);
      } else {
        this->stream.processed = 0;
      }
    }
  } else {
    error_last = WSAGetLastError();
    if (error_last != WSAEWOULDBLOCK) {
      error("send", error_last);
      net_tcp_close_handle(this);
    }
  }
}
void net_tcp_read_handle(net_tcp_t* this) {
  u64 remaining = this->stream.buffer_size - this->stream.processed;
  byte* buffer_start = this->stream.readable + this->stream.processed;
  i32 received = recv(this->socket, buffer_start, remaining, 0);
  if (received > 0) {
    this->stream.updatedAt = date_now();
    this->stream.processed += received;
    if (this->stream.length != this->stream.processed) {
      return;
    }
  } else if (received < 0) {
    error_last = WSAGetLastError();
    if (error_last == WSAEWOULDBLOCK) {
      return;
    } else if (error_last) {
      error("recv", error_last);
    }
  }
  this->task.type = TASK_TCP_CLOSING;
  this->handle(this, this->stream.readable, this->stream.processed, this->stream.context);
  memory_free(this->stream.readable);
  if (this->task.type == TASK_TCP_CLOSING) {
    net_tcp_close_handle(this);
  } else {
    this->stream.processed = 0;
  }
}

#endif