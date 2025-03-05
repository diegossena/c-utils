#include <sdk/tcp.h>
#ifdef PLATFORM_WINDOWS
#include <winsock2.h>

export error_t tcp_connect(tcp_t this, ip4_t ip4, net_port_t port, u64 timeout_ms) {
  u_long mode = 1;
  if (ioctlsocket(this, FIONBIO, &mode))
    return WSAGetLastError();
  struct sockaddr_in address = {
    .sin_family = AF_INET,
    .sin_port = port,
    .sin_addr.s_addr = ip4
  };
  error_t error = connect(this, (SOCKADDR*)&address, sizeof(address));
  if (timeout_ms) {
    error = WSAGetLastError();
    if (error != WSAEWOULDBLOCK)
      return error;
    const TIMEVAL timeval = { 0, timeout_ms * 1000 };
    fd_set writable;
    FD_ZERO(&writable);
    FD_SET(this, &writable);
    error = select(0, 0, &writable, 0, &timeval);
    if (error <= 0)
      return ERR_ETIMEDOUT;
  }
  mode = 0;
  if (ioctlsocket(this, FIONBIO, &mode))
    return WSAGetLastError();
  return ERR_SUCCESS;
}
export error_t tcp_read(tcp_t this, char* target, u64 size) {
  while (size) {
    i32 result = recv(this, target, size, 0);
    if (result == 0)
      return ERR_SUCCESS;
    if (result < 0)
      return WSAGetLastError();
    target += result;
    size -= result;
  }
  return ERR_SUCCESS;
}
export error_t tcp_send(tcp_t this, const char* buffer, u64 size) {
  while (size) {
    i32 result = send(this, buffer, size, 0);
    if (result < 0)
      return WSAGetLastError();
    buffer += result;
    size -= result;
  }
  return ERR_SUCCESS;
}

#endif