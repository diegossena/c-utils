#include "internal/platform.h"
#include "internal/net.h"
#include "internal/memory.h"

#include "sdk/net.tcp.h"
#include "sdk/assert.h"

#if PLATFORM_WINDOWS

#include <winsock2.h>

net_tcp* net_tcp_new() {
  net_tcp* this = memory_alloc0(sizeof(net_tcp));
  return this;
}
void net_tcp_free(net_tcp* this) {
  closesocket(this->socket);
  memory_free(this);
}

error_code net_tcp_connect(net_tcp* this, net_connect_opt* options) {
  assert(*options->host);
  assert(options->port > 0);
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
  socket_address.sin_family = (u16)options->family;
  socket_address.sin_port = htons(options->port);
  socket_address.sin_addr.s_addr = inet_addr(host);
  // create socket
  this->socket = socket(socket_address.sin_family, SOCK_STREAM, IPPROTO_TCP);
  if (this->socket == INVALID_SOCKET) {
    error("socket", WSAGetLastError());
    closesocket(this->socket);
    return error_last;
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
      closesocket(this->socket);
      return error_last;
    }
  }
  error_last = connect(this->socket, (SOCKADDR*)&socket_address, sizeof(socket_address));
  if (options->timeout) {
    if (error_last != WSAEWOULDBLOCK) {
      error("connect", error_last);
      closesocket(this->socket);
      return error_last;
    }
    TIMEVAL timeval = { 0, options->timeout * 1000 };
    fd_set readable, writable;
    FD_ZERO(&readable);
    FD_SET(this->socket, &readable);
    FD_ZERO(&writable);
    FD_SET(this->socket, &writable);
    error_last = select(0, &readable, &writable, NULL, &timeval);
    if (error_last <= 0) {
      error("select", ERR_ETIMEDOUT);
      closesocket(this->socket);
      return error_last;
    }
  } else if (error_last == SOCKET_ERROR) {
    error("connect", ERR_ETIMEDOUT);
    closesocket(this->socket);
    return error_last;
  }
  return ERR_SUCCESS;
}

error_code net_tcp_listen(net_tcp* this, net_address* address) {
  // Definir o endereço e a porta do servidor
  struct sockaddr_in  socket_address = {};
  socket_address.sin_family = address->family;
  socket_address.sin_port = htons(address->port);
  // create socket
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
  // Aceitar conexões e lidar com elas
  console_log_cstr("Servidor HTTP iniciado. Aguardando conexoes...");
  while (true) {
    SOCKET client_socket = accept(this->socket, 0, 0);
    if (client_socket == INVALID_SOCKET) {
      error_last = WSAGetLastError();
      if (error_last != WSAEWOULDBLOCK) {
        error("accept", error_last);
        return error_last;
      }
    }
  }
  return ERR_SUCCESS;
onerror:
  closesocket(this->socket);
  return error_last;
}

#endif