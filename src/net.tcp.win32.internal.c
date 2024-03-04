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

#include <winsock2.h>

void net_tcp_close_handle(net_tcp_t* this) {
  console_log_cstr("net_tcp_close_handle");
  closesocket((SOCKET)this->socket);
  queue_remove(&app_global.tasks, &this->task.queue);
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
  u64 remaining = this->stream.length - this->stream.processed;
  const byte* buffer_start = this->stream.buffer + this->stream.processed;
  i32 sent = send(this->socket, buffer_start, remaining, 0);
  if (sent >= 0) {
    this->stream.processed += sent;
    if (this->stream.length == this->stream.processed) {
      this->task.type = TASK_TCP_CLOSING;
      this->task.handle(this, this->stream.context);
      console_log("this->stream.processed=%llu", this->stream.processed);
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
  u64 remaining = this->stream.length - this->stream.processed;
  byte* buffer_start = this->stream.buffer + this->stream.processed;
  i32 received = recv(this->socket, buffer_start, remaining, 0);
  if (received >= 0) {
    this->stream.processed += received;
    if (this->stream.length == this->stream.processed) {
      this->task.type = TASK_TCP_CLOSING;
      this->task.handle(this, this->stream.buffer, this->stream.length, this->stream.context);
      memory_free(this->stream.buffer);
    }
  } else {
    error_last = WSAGetLastError();
    if (error_last != WSAEWOULDBLOCK) {
      this->task.type = TASK_TCP_CLOSING;
      error("send", error_last);
    }
  }
}

#endif