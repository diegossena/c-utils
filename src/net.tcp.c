#include "internal/net.tcp.h"
#include "internal/stream.h"

void net_tcp_on_connect(net_tcp_t* this, const net_connect_callback callback) {
  this->stream.connection_cb = callback;
}