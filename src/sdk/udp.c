#include <sdk/udp.h>

udp_t udp_new() {
  return _socket_new(SOCKET_TYPE_DGRAM);
}
void udp_free(udp_t this) {
  _socket_free(this);
}