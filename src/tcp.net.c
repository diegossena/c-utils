#include "internal/net.tcp.h"
#include "internal/stream.h"

void net_tcp_on_connect(net_tcp* this, const net_connect_callback callback) {
  this->tcp_stream.connection_cb = callback;
}