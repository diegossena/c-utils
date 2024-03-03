#include "internal/console.h"
#include "internal/net.h"
#include "internal/application.h"
#include "sdk/application.h"
#include "internal/app.task.h"
#include "internal/net.tcp.h"

i32 app_run() {
  while (!app_global.stop_flag) {
    app_task_t* it = (app_task_t*)app_global.app_queue_head;
    while (it) {
      switch (it->type) {
        case TASK_TCP_LISTEN: net_tcp_listen_handle((net_tcp_t*)it); break;
        case TASK_TCP_CONNECT: net_tcp_connect_handle((net_tcp_t*)it); break;
      }
      it = (app_task_t*)it->handle_queue.next;
    }
  }
  net_shutdown();
  return 0;
}