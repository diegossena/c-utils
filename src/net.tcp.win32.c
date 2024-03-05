#include "internal/platform.h"

#if PLATFORM_WINDOWS

// app_global
#include "internal/application.h"
// net_tcp_t
#include "internal/net.tcp.h"
// memory_alloc0
#include "internal/memory.h"
// assert
#include "sdk/assert.h"
// socket
#include <winsock2.h>

void net_tcp_free(net_tcp_t* this) {
  closesocket((SOCKET)this->socket);
  queue_remove(&app_global.tasks, &this->task.queue);
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
  if (sock > app_global.max_fd) {
    app_global.max_fd = sock + 1;
  }
  console_log("net_tcp_create_socket=%d", sock);
  return sock;
}

error_code net_tcp_ip4_connect(net_tcp_t* this, const char* host, u16 port, net_tcp_on_connect_cb callback) {
  assert(port > 0);
  net_addr_t ipv4_addr = {};
  ipv4_addr.family = AF_INET;
  ipv4_addr.port = htons(port);
  ipv4_addr.in_addr.S_addr = net_host_resolve(host);
  this->socket = net_tcp_create_socket(&ipv4_addr);
  if (this->socket < 0) {
    goto onerror;
  }
  // connect
  error_last = connect(this->socket, (SOCKADDR*)&ipv4_addr, sizeof(ipv4_addr));
  if (error_last == SOCKET_ERROR) {
    error_last = WSAGetLastError();
    if (error_last != WSAEWOULDBLOCK) {
      error("connect", error_last);
      goto onerror;
    }
  }
  this->task.type = TASK_TCP_CONNECTING;
  this->task.handle = callback;
  return ERR_SUCCESS;
onerror:
  closesocket(this->socket);
  return error_last;
}

#endif