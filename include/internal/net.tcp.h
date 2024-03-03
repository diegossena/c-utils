#include "internal/stream.h"
#include "internal/net.h"
#include "sdk/net.tcp.h"

class net_tcp_t {
  stream_t stream;
  net_addr_t addr;
} net_tcp_t;

void net_tcp_listen_handle(net_tcp_t*);
void net_tcp_connect_handle(net_tcp_t*);