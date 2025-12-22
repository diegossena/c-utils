#include <sdk/tcp_server.h>
#ifdef PLATFORM_WINDOWS
#include <sdk/net.win32.h>
#include <winsock2.h>
#include <mswsock.h>

void tcp_listen(tcp_server_t* this, u16 port) {
  i32 result;
  this->socket = _socket_new(SOCKET_STREAM);
  if (this->socket == INVALID_SOCKET) {
    result = WSAGetLastError();
    error(result, "socket");
    exit(result);
  }
  CreateIoCompletionPort((HANDLE)this->socket, _iocp, (ULONG_PTR)this, 0);
  const net_address_t address = {
    .family = NET_FAMILY_IPV4,
    .net_port = net_port_short(port),
    .ip4 = INADDR_ANY
  };
  result = bind(this->socket, (struct sockaddr*)&address, sizeof(address));
  if (result < 0) {
    result = WSAGetLastError();
    error(result, "bind");
    goto onservererror;
  }
  result = listen(this->socket, 0);
  if (result < 0) {
    result = WSAGetLastError();
    error(result, "listen");
    goto onservererror;
  }
  this->accept = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (this->accept == INVALID_SOCKET) {
    error(WSAGetLastError(), "accept socket");
    goto onservererror;
  }
  u32 bytes;
  OVERLAPPED overlapped;
  console_write("socket %d\n", this->socket);
  console_write("accept %d\n", this->accept);
  result = AcceptEx(
    this->socket,
    this->accept,
    this->buffer,
    0,
    sizeof(net_address_t) + 16,
    sizeof(net_address_t) + 16,
    (DWORD*)&bytes,
    &overlapped
  );
  if (result == 0) {
    error(WSAGetLastError(), "AcceptEx");
    goto onclienterror;
  }
  HANDLE handle = CreateIoCompletionPort((HANDLE)this->accept, _iocp, (ULONG_PTR)this, 0);
  if (handle == NULL) {
    error(GetLastError(), "tcp_server CreateIoCompletionPort");
    goto onclienterror;
  }
  return;
onclienterror:
  closesocket(this->accept);
onservererror:
  closesocket(this->socket);
  WSACleanup();
  exit(result);
}

#endif