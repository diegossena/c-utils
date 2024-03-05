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
static char buffer_zero [] = "";
void CALLBACK net_tcp_client_read_handle_callback(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags) {
  console_log_cstr("net_tcp_client_read_handle_callback");
  net_tcp_client_t* this = (net_tcp_client_t*)lpOverlapped->hEvent;
  if (dwError) {
    error("WSARecv", error_last);
  } else {
    net_tcp_client_close_handle(this);
  }
}

void net_tcp_server_listen_handle(net_tcp_server_t* this) {
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
    client->task.type = TASK_TCP_CLIENT_CLOSING;
    this->task.handle(client);
    if (client->task.type == TASK_TCP_CLIENT_CLOSING) {
      closesocket(client_socket);
      memory_free(client);
    } else {
      ++this->client_count;
      queue_push(&app_global.tasks, &client->task.queue);
    }
  }
}

#endif