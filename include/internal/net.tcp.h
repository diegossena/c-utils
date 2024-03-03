#include "internal/stream.h"
#include "sdk/net.tcp.h"

class net_tcp_t {
  stream_t stream;
  u64 socket;
} net_tcp_t;

void net_tcp_listen_handle(net_tcp_t*);