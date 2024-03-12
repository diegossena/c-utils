#include <sdk/net/net.tcp.server.h>
#include <sdk/application/application.h>

typedef struct net_tcp_server_t {
  task_t task;
  application_t* app;
  u64 socket;
  u64 client_count;
  net_tcp_on_connection_cb handle;
} net_tcp_server_t;

net_tcp_server_t* net_tcp_server_new(application_t* app) {
  net_tcp_server_t* this = memory_alloc0(sizeof(net_tcp_server_t));
  this->app = app;
  this->task.type = TASK_TCP_SERVER_CLOSING;
  task_register(this, app);
  return this;
}