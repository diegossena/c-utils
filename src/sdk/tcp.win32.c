#include <sdk/tcp.h>
#ifdef PLATFORM_WINDOWS
#include <winsock2.h>

export error_t tcp_connect(tcp_t this, ip4_t ip4, net_port_t port, u64 timeout_ms) {
  u_long mode = 1;
  if (ioctlsocket(this, FIONBIO, &mode) != NO_ERROR) {
    return net_error();
  }
  struct sockaddr_in address = {
    .sin_family = AF_INET,
    .sin_port = port,
    .sin_addr.s_addr = ip4
  };
  error_t error = connect(this, (SOCKADDR*)&address, sizeof(address));
  if (timeout_ms) {
    error = net_error();
    if (error != WSAEWOULDBLOCK)
      return error;
    TIMEVAL timeval = { 0, timeout_ms * 1000 };
    fd_set readable, writable;
    FD_ZERO(&readable);
    FD_SET(this, &readable);
    FD_ZERO(&writable);
    FD_SET(this, &writable);
    error = select(0, &readable, &writable, NULL, &timeval);
    if (error <= 0)
      return ERR_ETIMEDOUT;
  }
  mode = 0;
  if (ioctlsocket(this, FIONBIO, &mode) != NO_ERROR) {
    return net_error();
  }
  return ERR_SUCCESS;
}
export i32 tcp_read_stream(tcp_t this, char* target, u64 size) {
  return recv(this, target, size, 0);
}
export i32 tcp_send_stream(tcp_t this, const char* stream, u64 size) {
  return send(this, stream, size, 0);
}

#endif