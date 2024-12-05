#include <sdk/tcp.h>

export tcp_t tcp_new() {
  return _socket_new(SOCKET_TYPE_STREAM);
}
export void tcp_free(tcp_t this) {
  _socket_free(this);
}