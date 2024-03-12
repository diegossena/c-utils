#include <sdk/net/net.tcp.server.h>

net_tcp_server_t* net_tcp_server_new(application_t* app) {
  net_tcp_server_t* this = memory_alloc0(sizeof(net_tcp_server_t));
  this->app = app;
  this->task.type = TASK_TCP_SERVER_CLOSING;
  task_register(this, app);
  return this;
}