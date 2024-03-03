#include "internal/platform.h"

#if PLATFORM_WINDOWS

#include "internal/net.tcp.h"
#include "internal/memory.h"
#include "sdk/assert.h"
#include <winsock2.h>

net_tcp_t* net_tcp_new() {
  net_tcp_t* this = memory_alloc0(sizeof(net_tcp_t));
  stream_register(&this->stream, TASK_NONE);
  return this;
}
void net_tcp_free(net_tcp_t* this) {
  closesocket(this->socket);
  memory_free(this);
}

error_code __net_tcp_create_socket(net_tcp_t* this) {
  this->socket = socket(this->addr.family, SOCK_STREAM, IPPROTO_TCP);
  if (this->socket == INVALID_SOCKET) {
    error("socket", WSAGetLastError());
    return error_last;
  }
  if (this->socket > app_global.max_fd) {
    app_global.max_fd = this->socket;
  }
  return ERR_SUCCESS;
}

error_code net_tcp_ip4_addr(net_tcp_t* this, u16 port, const char* host) {
  assert(port > 0);
  this->addr.family = AF_INET;
  this->addr.sin_port = htons(port);
  // resolve host
  if (host) {
    if (*host >= '0' && *host <= '9') {
      this->addr.in_addr.S_addr = inet_addr(host);
    } else {
      struct hostent* remote_host = gethostbyname(host);
      if (remote_host == NULL) {
        error("gethostbyname", WSAGetLastError());
        return error_last;
      }
      this->addr.in_addr.S_addr = *(u32*)remote_host->h_addr_list[0];
    }
  } else {
    this->addr.in_addr.S_addr = INADDR_ANY;
  }
  return ERR_SUCCESS;
}
error_code net_tcp_connect(net_tcp_t* this) {
  if (__net_tcp_create_socket(this)) {
    goto onerror;
  }
  /**
   * If iMode = 0, blocking is enabled;
   * If iMode != 0, non-blocking mode is enabled.
   */
  u_long mode = 1;
  error_last = ioctlsocket(this->socket, FIONBIO, &mode);
  if (error_last != NO_ERROR) {
    error("ioctlsocket", error_last);
    goto onerror;
  }
  // connect
  error_last = connect(this->socket, (SOCKADDR*)&this->addr, sizeof(this->addr));
  if (error_last == SOCKET_ERROR) {
    error_last = WSAGetLastError();
    if (error_last != WSAEWOULDBLOCK) {
      error("connect", error_last);
      goto onerror;
    }
  }
  this->stream.task.type = TASK_TCP_CONNECT;
  return ERR_SUCCESS;
onerror:
  closesocket(this->socket);
  return error_last;
}

error_code net_tcp_listen(net_tcp_t* this) {
  if (__net_tcp_create_socket(this)) {
    goto onerror;
  }
  // Vincular o socket ao endereço e porta
  if (bind(this->socket, (SOCKADDR*)&this->addr, sizeof(this->addr)) == SOCKET_ERROR) {
    error("bind", WSAGetLastError());
    goto onerror;
  }
  // Escutar por conexões
  if (listen(this->socket, SOMAXCONN) == SOCKET_ERROR) {
    error("listen", WSAGetLastError());
    goto onerror;
  }
  // Set the socket to nonblocking mode
  u_long mode = 1;
  if (ioctlsocket(this->socket, FIONBIO, &mode) == SOCKET_ERROR) {
    error("ioctlsocket", WSAGetLastError());
    goto onerror;
  }
  this->stream.task.type = TASK_TCP_LISTEN;
  // Aceitar conexões e lidar com elas
  return ERR_SUCCESS;
onerror:
  closesocket(this->socket);
  return error_last;
}

#endif