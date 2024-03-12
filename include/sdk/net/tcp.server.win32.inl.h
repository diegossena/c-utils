#include <sdk/platform/platform.h>

#if PLATFORM_WINDOWS

#include <sdk/net/net.tcp.server.h>
#include <sdk/net/net.tcp.client.h>
#include <sdk/net/net.tcp.h>

void net_tcp_server_free(net_tcp_server_t* this) {
  closesocket(this->socket);
  task_unregister(this);
  memory_free(this);
}

error_code net_tcp_server_ip4_listen(net_tcp_server_t* this, u16 port, net_tcp_on_connection_cb callback) {
  assert(port > 0);
  net_addr_t ipv4_addr = {
    .family = AF_INET,
    .port = htons(port)
  };
  this->socket = net_tcp_create_socket(&ipv4_addr);
  if (this->socket < 0) {
    goto onerror;
  }
  // Vincular o socket ao endereço e porta
  if (bind(this->socket, (SOCKADDR*)&ipv4_addr, sizeof(ipv4_addr)) == SOCKET_ERROR) {
    error("bind", WSAGetLastError());
    goto onerror;
  }
  // Escutar por conexões
  if (listen(this->socket, SOMAXCONN) == SOCKET_ERROR) {
    error("listen", WSAGetLastError());
    goto onerror;
  }
  this->task.type = TASK_TCP_SERVER_LISTENING;
  this->handle = (void*)callback;
  // Aceitar conexões e lidar com elas
  return ERR_SUCCESS;
onerror:
  closesocket(this->socket);
  return error_last;
}

void net_tcp_server_close_handle(net_tcp_server_t* this) {
  if (!this->client_count) {
    net_tcp_server_free(this);
  }
}

void CALLBACK net_tcp_client_read_handle_callback(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags) {
  console_log_cstr("net_tcp_client_read_handle_callback");
  net_tcp_client_t* this = (net_tcp_client_t*)lpOverlapped->hEvent;
  if (dwError) {
    error("WSARecv", error_last);
  } else {
    net_tcp_client_free(this);
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
    this->handle(client);
    if (client->task.type == TASK_TCP_CLIENT_CLOSING) {
      closesocket(client_socket);
      memory_free(client);
    } else {
      ++this->client_count;
      task_register(client, this->app);
    }
  }
}

#endif