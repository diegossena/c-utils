#include <sdk/platform/platform.h>

#if PLATFORM_WINDOWS

#include <sdk/net/net.tcp.client.h>

void net_tcp_client_free(net_tcp_client_t* this) {
  shutdown(this->socket, FD_WRITE | FD_READ);
  task_unregister(this);
  closesocket(this->socket);
  --this->server->client_count;
  memory_free(this);
}
void net_tcp_client_read_handle(net_tcp_client_t* this) {
  u64 remaining = this->stream.buffer_size - this->stream.processed;
  byte* buffer_start = this->stream.readable + this->stream.processed;
  u64 time = date_now();
  i32 received = recv(this->socket, buffer_start, remaining, 0);
  u64 deltaTime = date_now() - time;
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
  this->task.type = TASK_TCP_CLIENT_CLOSING;
  this->handle(this, this->stream.readable, this->stream.processed, this->stream.context);
  memory_free(this->stream.readable);
  if (this->task.type == TASK_TCP_CLIENT_CLOSING) {
    net_tcp_client_free(this);
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
      this->task.type = TASK_TCP_CLIENT_CLOSING;
      this->handle(this, this->stream.context);
      if (this->task.type == TASK_TCP_CLIENT_CLOSING) {
        net_tcp_client_free(this);
      } else {
        this->stream.processed = 0;
      }
    }
  } else {
    error_last = WSAGetLastError();
    if (error_last && error_last != WSAEWOULDBLOCK) {
      error("send", error_last);
      net_tcp_client_free(this);
    }
  }
}

#endif