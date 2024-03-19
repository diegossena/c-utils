#include <sdk/platform.h>

#if PLATFORM_WINDOWS

#include <sdk/net/tcp/server.h>

void net_tcp_server_free(net_tcp_server_t* this) {
  closesocket(this->socket);
  emitter_off(&this->__listener);
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
  this->__listener.callback = (listener_t)net_tcp_server_listen_handle;
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
    client->__listener.callback = (listener_t)net_tcp_client_free;
    this->handle(client);
    if (client->__listener.callback == (listener_t)net_tcp_client_free) {
      closesocket(client_socket);
      memory_free(client);
    } else {
      client->__listener.context = client;
      ++this->client_count;
      emitter_on(&this->app->__tasks, &client->__listener);
    }
  }
}
void net_tcp_client_free(net_tcp_client_t* this) {
  shutdown(this->socket, FD_WRITE | FD_READ);
  emitter_off(&this->__listener);
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
  this->__listener.callback = (listener_t)net_tcp_client_free;
  this->handle(this, this->stream.readable, this->stream.processed, this->stream.context);
  memory_free(this->stream.readable);
  if (this->__listener.callback == (listener_t)net_tcp_client_free) {
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
      this->__listener.callback = (listener_t)net_tcp_client_free;
      this->handle(this, this->stream.context);
      if (this->__listener.callback == (listener_t)net_tcp_client_free) {
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