#include "internal/platform.h"

#if PLATFORM_WINDOWS

// app_global
#include "internal/application.h"
// net_tcp_server_t
#include "internal/net.tcp.server.h"
// net_tcp_client_t
#include "internal/net.tcp.client.h"
// memory_alloc0
#include "internal/memory.h"
// TODO: remove this line
#include "sdk/console.h" 
// accept, WSAGetLastError
#include <winsock2.h>

void net_tcp_server_close_handle(net_tcp_server_t* this) {
  if (!this->client_count) {
    closesocket((SOCKET)this->socket);
    queue_remove(&app_global.tasks, &this->task.queue);
    memory_free(this);
  }
}

void net_tcp_server_listen_handle(net_tcp_server_t* this) {
  console_log_cstr("net_tcp_server_listen_handle");
  SOCKET client_socket = accept((SOCKET)this->socket, 0, 0);
  if (client_socket == INVALID_SOCKET) {
    error_last = WSAGetLastError();
    if (error_last != WSAEWOULDBLOCK) {
      error("accept", error_last);
    }
  } else {
    net_tcp_client_t* client = memory_alloc0(sizeof(net_tcp_client_t));
    client->server = this;
    client->socket = client_socket;
    client->task.type = TASK_TCP_CLOSING;
    this->task.handle(client);
    ++this->client_count;
  }
}

#endif