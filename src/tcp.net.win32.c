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

error_code net_tcp_connect(net_tcp_t* this, net_connect_opt* options) {
  assert(*options->host);
  assert(options->adress.port > 0);
  // resolve address
  struct hostent* remoteHost = gethostbyname(options->host);
  if (remoteHost == NULL) {
    error("gethostbyname", WSAGetLastError());
    return error_last;
  }
  struct in_addr addr;
  addr.s_addr = *(u32*)remoteHost->h_addr_list[0];
  const char* host = (const char*)inet_ntoa(addr);
  struct sockaddr_in socket_address = {};
  socket_address.sin_family = (u16)options->adress.family;
  socket_address.sin_port = htons(options->adress.port);
  socket_address.sin_addr.s_addr = inet_addr(host);
  // create socket
  this->socket = socket(socket_address.sin_family, SOCK_STREAM, IPPROTO_TCP);
  if (this->socket == INVALID_SOCKET) {
    error("socket", WSAGetLastError());
    goto onerror;
  }
  /**
   * If iMode = 0, blocking is enabled;
   * If iMode != 0, non-blocking mode is enabled.
   */
  u_long iMode = 1;
  if (options->timeout) {
    error_last = ioctlsocket(this->socket, FIONBIO, &iMode);
    if (error_last != NO_ERROR) {
      error("ioctlsocket", error_last);
      goto onerror;
    }
  }
  error_last = connect(this->socket, (SOCKADDR*)&socket_address, sizeof(socket_address));
  if (error_last == SOCKET_ERROR) {
    error_last = WSAGetLastError();
    if (error_last != WSAEWOULDBLOCK) {
      error("connect", error_last);
      goto onerror;
    }
  }
  this->stream.task.type = TASK_TCP_CONNECTING;
  // if (options->timeout) {
  //   if (error_last != WSAEWOULDBLOCK) {
  //     error("connect", error_last);
  //     goto onerror;
  //   }
  //   TIMEVAL timeval = { 0, options->timeout * 1000 };
  //   fd_set readable, writable;
  //   FD_ZERO(&readable);
  //   FD_SET(this->socket, &readable);
  //   FD_ZERO(&writable);
  //   FD_SET(this->socket, &writable);
  //   error_last = select(0, &readable, &writable, NULL, &timeval);
  //   if (error_last <= 0) {
  //     error("select", ERR_ETIMEDOUT);
  //     goto onerror;
  //   }
  // } else if (error_last == SOCKET_ERROR) {
  //   error("connect", ERR_ETIMEDOUT);
  //   goto onerror;
  // }
  return ERR_SUCCESS;
onerror:
  closesocket(this->socket);
  return error_last;
}

error_code net_tcp_listen(net_tcp_t* this, net_address_t* address) {
  // Definir o endereço e a porta do servidor
  struct sockaddr_in  socket_address = {};
  socket_address.sin_family = address->family;
  socket_address.sin_addr.s_addr = INADDR_ANY;
  socket_address.sin_port = htons(address->port);
  // Create socket
  this->socket = socket(socket_address.sin_family, SOCK_STREAM, IPPROTO_TCP);
  if (this->socket == INVALID_SOCKET) {
    error("socket", WSAGetLastError());
    goto onerror;
  }
  // Vincular o socket ao endereço e porta
  if (bind(this->socket, (SOCKADDR*)&socket_address, sizeof(socket_address)) == SOCKET_ERROR) {
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