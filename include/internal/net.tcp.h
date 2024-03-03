#include "internal/stream.h"
#include "sdk/net.tcp.h"

class net_tcp {
  stream tcp_stream;
  u64 socket;
} net_tcp;

void net_tcp_listen_handle(net_tcp*);