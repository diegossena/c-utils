// app_run, app_global
#include "internal/application.h"
// net_tcp_listen_handle, net_tcp_connect_handle, net_tcp_t
#include "internal/net.tcp.h"
// task_t
#include "internal/task.h"
// memory_free
#include "internal/memory.h"
// console_log_cstr
#include "sdk/console.h" // TODO: remove this line

i32 app_run() {
  task_t* it = (task_t*)app_global.tasks;
  while (it) {
    while (it) {
      switch (it->type) {
        case TASK_TCP_CONNECT: net_tcp_connect_handle((net_tcp_t*)it); break;
        case TASK_TCP_LISTEN: net_tcp_listen_handle((net_tcp_t*)it); break;
      }
      it = (task_t*)it->queue.next;
    }
    it = (task_t*)app_global.tasks;
  }
  net_shutdown();
  return 0;
}