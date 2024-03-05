// net_tcp_t
#include "sdk/net.tcp.h"
// task_t
#include "internal/task.h"
// net_addr_t
#include "internal/net.h"
// net_tcp_server_t
#include "internal/net.tcp.server.h"
// stream_t stream;
#include "internal/stream.h"

typedef struct net_tcp_client_t {
  // extends net_tcp_t
  task_t task;
  stream_t stream;
  u64 socket;
  // fields
  net_addr_t addr;
  net_tcp_server_t* server;
} net_tcp_client_t;

void net_tcp_connect_handle(net_tcp_t*);