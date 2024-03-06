// net_addr_t
#include "internal/net.h"
// net_tcp_t
#include "sdk/net.tcp.h"
// task_t
#include "internal/task.h"
// stream_t
#include "internal/stream.h"

typedef struct net_tcp_t {
  task_t task;
  stream_t stream;
  u64 socket;
  task_handle handle;
} net_tcp_t;

u64 net_tcp_create_socket(const net_addr_t*);

void net_tcp_connect_handle(net_tcp_t*);
void net_tcp_close_handle(net_tcp_t*);
void net_tcp_read_handle(net_tcp_t*);
void net_tcp_write_handle(net_tcp_t*);