#include "internal/platform.h"

#if PLATFORM_WINDOWS

// app_global
#include "internal/application.h"
// net_tcp_server_t
#include "internal/net.tcp.server.h"
// net_tcp_create_socket
#include "internal/net.tcp.h"
// memory_alloc0
#include "internal/memory.h"
// assert
#include "sdk/assert.h"
// socket
#include <winsock2.h>

void net_tcp_server_free(net_tcp_server_t* this) {
  task_unregister(&this->task);
  closesocket(this->socket);
  memory_free(this);
}

error_code net_tcp_server_ip4_listen(net_tcp_server_t* this, u16 port, net_tcp_on_connection_cb callback) {
  assert(port > 0);
  net_addr_t ipv4_addr = {
    .family = AF_INET,
    .port = htons(port)
  };
  if (this->socket = net_tcp_create_socket(&ipv4_addr)) {
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
  this->task.type = TASK_TCP_SERVER_LISTENING;
  this->task.handle = (void*)callback;
  // Aceitar conexões e lidar com elas
  return ERR_SUCCESS;
onerror:
  closesocket(this->socket);
  return error_last;
}

#endif