#include <sdk/udp.h>

export udp_t udp_new() {
  return _socket_new(SOCKET_TYPE_DGRAM);
}
export void udp_free(udp_t this) {
  _socket_free(this);
}