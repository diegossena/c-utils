#include <sdk/platform.h>

#if PLATFORM_WINDOWS

#include <sdk/net/tcp.h>
#include <sdk/date/date.h>

void net_tcp_free(net_tcp_t* this) {
  shutdown(this->__socket, FD_WRITE | FD_READ);
  closesocket((SOCKET)this->__socket);
  task_unregister(this);
  memory_free(this);
}

u64 net_tcp_create_socket(const net_addr_t* addr) {
  u64 sock = socket(addr->family, SOCK_STREAM, IPPROTO_TCP);
  if (sock == INVALID_SOCKET) {
    error("socket", WSAGetLastError());
    return sock;
  }
  /**
   * If iMode = 0, blocking is enabled;
   * If iMode != 0, non-blocking mode is enabled.
   */
  u_long mode = 1;
  if (ioctlsocket(sock, FIONBIO, &mode) == SOCKET_ERROR) {
    error("ioctlsocket", WSAGetLastError());
    return sock;
  }
  if (sock > __application_max_fd) {
    __application_max_fd = sock + 1;
  }
  return sock;
}

error_code net_tcp_ip4_connect(net_tcp_t* this, const char* host, u16 port, net_tcp_on_connect_cb callback) {
  assert(port > 0);
  net_addr_t ipv4_addr = {};
  ipv4_addr.family = AF_INET;
  ipv4_addr.port = htons(port);
  ipv4_addr.in_addr.S_addr = net_host_resolve(host);
  this->__socket = net_tcp_create_socket(&ipv4_addr);
  if (this->__socket < 0) {
    goto onerror;
  }
  // connect
  error_last = connect(this->__socket, (SOCKADDR*)&ipv4_addr, sizeof(ipv4_addr));
  if (error_last == SOCKET_ERROR) {
    error_last = WSAGetLastError();
    if (error_last != WSAEWOULDBLOCK) {
      error("connect", error_last);
      goto onerror;
    }
  }
  this->__task.type = TASK_TCP_CONNECTING;
  this->__handle = callback;
  this->__stream.updatedAt = date_now();
  return ERR_SUCCESS;
onerror:
  closesocket(this->__socket);
  return error_last;
}

void net_tcp_connect_handle(net_tcp_t* this) {
  struct timeval timeout = {};
  fd_set writable = {};
  FD_SET(this->__socket, &writable);
  error_last = select(__application_max_fd, null, &writable, null, &timeout);
  if (error_last > 0) {
    // connected
    this->__task.type = TASK_TCP_CLOSING;
    this->__handle(this);
    if (this->__task.type == TASK_TCP_CLOSING) {
      net_tcp_free(this);
    } else {
      this->__stream.updatedAt = date_now();
    }
    return;
  } else if (error_last == 0) {
    u64 deltaTime = date_now() - this->__stream.updatedAt;
    if (deltaTime > DEFAULT_TIMEOUT) {
      error_last = ERR_ETIMEDOUT;
    } else {
      return;
    }
  } else {
    error_last = WSAGetLastError();
  }
  // onerror
  error("select", error_last);
  net_tcp_free(this);
}
void net_tcp_write_handle(net_tcp_t* this) {
  u64 remaining = this->__stream.length - this->__stream.processed;
  const byte* buffer_start = this->__stream.writable + this->__stream.processed;
  i32 sent = send(this->__socket, buffer_start, remaining, 0);
  if (sent > 0) {
    this->__stream.updatedAt = date_now();
    this->__stream.processed += sent;
    if (this->__stream.length == this->__stream.processed) {
      memory_free(this->__stream.writable);
      this->__task.type = TASK_TCP_CLOSING;
      this->__handle(this, this->__stream.context);
      if (this->__task.type == TASK_TCP_CLOSING) {
        net_tcp_free(this);
      } else {
        this->__stream.processed = 0;
      }
    }
    return;
  }
  u64 deltaTime = date_now() - this->__stream.updatedAt;
  console_log("deltaTime=%llu", deltaTime);
  if (deltaTime > DEFAULT_TIMEOUT) {
    error_last = ERR_ETIMEDOUT;
  } else if (error_last < 0) {
    error_last = WSAGetLastError();
    if (error_last == WSAEWOULDBLOCK) return;
  } else {
    return;
  }
  // onerror
  error("send", error_last);
  net_tcp_free(this);
}
void net_tcp_read_handle(net_tcp_t* this) {
  u64 remaining = this->__stream.buffer_size - this->__stream.processed;
  byte* buffer_start = this->__stream.readable + this->__stream.processed;
  i32 received = recv(this->__socket, buffer_start, remaining, 0);
  u64 deltaTime = date_now() - this->__stream.updatedAt;
  if (received > 0) {
    this->__stream.processed += received;
    if (this->__stream.length == this->__stream.processed) {
      goto readend;
    }
    this->__stream.updatedAt = date_now();
    return;
  } else if (received == 0) {
    goto readend;
  }
  if (deltaTime > DEFAULT_TIMEOUT) {
    error_last = ERR_ETIMEDOUT;
  } else {
    error_last = WSAGetLastError();
    if (error_last == WSAEWOULDBLOCK) return;
  }
  // onerror
  error("recv", ERR_ETIMEDOUT);
readend:
  this->__task.type = TASK_TCP_CLOSING;
  this->__handle(this, this->__stream.readable, this->__stream.processed, this->__stream.context);
  memory_free(this->__stream.readable);
  if (this->__task.type == TASK_TCP_CLOSING) {
    net_tcp_free(this);
  } else {
    this->__stream.processed = 0;
  }
}

#endif