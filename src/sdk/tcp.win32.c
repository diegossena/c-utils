#include <sdk/tcp.h>
#ifdef PLATFORM_WINDOWS
#include <winsock2.h>

export error_t tcp_connect(tcp_t this, ip4_t ip4, net_port_t port, u64 timeout_ms) {
  u_long mode = 1;
  if (ioctlsocket(this, FIONBIO, &mode) != NO_ERROR) {
    return WSAGetLastError();
  }
  struct sockaddr_in address = {
    .sin_family = AF_INET,
    .sin_port = port,
    .sin_addr.s_addr = ip4
  };
  error_t result = connect(this, (SOCKADDR*)&address, sizeof(address));
  if (timeout_ms) {
    result = WSAGetLastError();
    if (result != WSAEWOULDBLOCK)
      return result;
    TIMEVAL timeval = { 0, timeout_ms * 1000 };
    fd_set readable, writable;
    FD_ZERO(&readable);
    FD_SET(this, &readable);
    FD_ZERO(&writable);
    FD_SET(this, &writable);
    result = select(0, &readable, &writable, NULL, &timeval);
    if (result <= 0)
      return ERR_ETIMEDOUT;
  }
  return ERR_SUCCESS;
}

#endif