#include "internal/platform.h"

#if PLATFORM_WINDOWS

#include "internal/net.tcp.client.h"
// app_global
#include "internal/application.h"
// date_now
#include "sdk/date.h"
// memory_free
#include "internal/memory.h"
// timeval, fd_set, FD_SET, select, WSAGetLastError
#include <winsock2.h>
// TODO: remove this line
#include "sdk/console.h"

void net_tcp_client_read_handle(net_tcp_client_t* this) {
  console_log("net_tcp_client_read_handle");
  u64 remaining = this->stream.buffer_size - this->stream.processed;
  byte* buffer_start = this->stream.readable + this->stream.processed;
  i32 received = recv(this->socket, buffer_start, remaining, 0);
  if (received < 0) {
    error_last = WSAGetLastError();
    if (error_last) {
      if (error_last != WSAEWOULDBLOCK) {
        error("recv", error_last);
        goto endstream;
      }
      if (this->stream.processed > 0) {
        goto endstream;
      }
    } else {
      goto endstream;
    }
  } else if (received > 0) {
    this->stream.updatedAt = date_now();
    this->stream.processed += received;
    if (this->stream.length == this->stream.processed) {
      goto endstream;
    }
  } else {
    goto endstream;
  }
  return;
endstream:
  this->task.type = TASK_TCP_CLIENT_CLOSING;
  this->task.handle(this, this->stream.readable, this->stream.processed, this->stream.context);
  memory_free(this->stream.readable);
  if (this->task.type == TASK_TCP_CLIENT_CLOSING) {
    net_tcp_client_close_handle(this);
  } else {
    this->stream.processed = 0;
  }
}
void net_tcp_client_write_handle(net_tcp_client_t* this) {
  u64 remaining = this->stream.length - this->stream.processed;
  const byte* buffer_start = this->stream.writable + this->stream.processed;
  i32 sent = send(this->socket, buffer_start, remaining, 0);
  if (sent > 0) {
    this->stream.updatedAt = date_now();
    this->stream.processed += sent;
    if (this->stream.length == this->stream.processed) {
      memory_free(this->stream.writable);
      this->stream.processed = 0;
      this->task.type = TASK_TCP_CLOSING;
      this->task.handle(this, this->stream.context);
    }
  } else {
    error_last = WSAGetLastError();
    if (error_last && error_last != WSAEWOULDBLOCK) {
      error("send", error_last);
      net_tcp_client_close_handle(this);
    }
  }
}

#endif