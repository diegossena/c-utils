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
  console_log_cstr("net_tcp_close_handle");
  queue_remove(&app_global.tasks, (queue_t*)&this);
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
    this->task.handle(this);
  } else if (error_last == SOCKET_ERROR) {
    error_last = WSAGetLastError();
    if (error_last != WSAEWOULDBLOCK) {
      this->task.type = TASK_TCP_CLOSING;
      error("select", error_last);
      return;
    }
    // connecting...
  }
}
void net_tcp_write_handle(net_tcp_t* this) {
  console_log_cstr("net_tcp_write_handle");
  u64 remaining = this->stream.length - this->stream.processed;
  const byte* buffer_start = this->stream.buffer + this->stream.processed;
  i32 sent = send(this->socket, buffer_start, remaining, 0);
  if (sent >= 0) {
    this->stream.processed += sent;
    if (this->stream.length == this->stream.processed) {
      this->stream.processed = 0;
      this->task.type = TASK_TCP_CLOSING;
      this->task.handle(this, this->stream.context);
    }
  } else {
    error_last = WSAGetLastError();
    if (error_last != WSAEWOULDBLOCK) {
      this->task.type = TASK_TCP_CLOSING;
      error("send", error_last);
    }
  }
}
void net_tcp_read_handle(net_tcp_t* this) {
  u64 remaining = this->stream.size - this->stream.processed;
  byte* buffer_start = this->stream.buffer + this->stream.processed;
  i32 received = recv(this->socket, buffer_start, remaining, 0);
  console_log("received=%d", received);
  console_log("remaining=%llu", remaining);
  console_log("this->stream.length=%llu", this->stream.length);
  console_log("this->stream.processed=%llu", this->stream.processed);
  if (received > 0) {
    this->stream.updatedAt = date_now();
    this->stream.processed += received;
    if (this->stream.length == this->stream.processed) {
      goto endstream;
    }
  } else {
    error_last = WSAGetLastError();
    if (error_last != WSAEWOULDBLOCK) {
      this->task.type = TASK_TCP_CLOSING;
      error("recv", error_last);
    } else if (!this->stream.length && (date_now() - this->stream.updatedAt) > 1000) {
      goto endstream;
    }
  }
  return;
endstream:
  this->stream.processed = 0;
  this->task.type = TASK_TCP_CLOSING;
  this->task.handle(this, this->stream.buffer, this->stream.length, this->stream.context);
  memory_free(this->stream.buffer);
}

#endif