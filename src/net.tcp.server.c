// net_tcp_server_t
#include "internal/net.tcp.server.h"
// memory_alloc0
#include "internal/memory.h"

net_tcp_server_t* net_tcp_server_new() {
  net_tcp_server_t* this = memory_alloc0(sizeof(net_tcp_server_t));
  this->task.type = TASK_TCP_SERVER_CLOSING;
  task_register(&this->task);
  return this;
}