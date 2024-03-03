// net_tcp_on_connect, net_tcp_t, task_handle
#include "internal/net.tcp.h"

void net_tcp_on_connect(net_tcp_t* this, net_tcp_on_connect_cb callback) {
  this->stream.task.handle = (task_handle)callback;
}