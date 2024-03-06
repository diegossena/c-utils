// net_tcp_connect_handle, net_tcp_t
#include "internal/net.tcp.h"
// app_run, app_global
#include "internal/application.h"
// net_tcp_server_listen_handle
#include "internal/net.tcp.server.h"
// net_tcp_client_close_handle
#include "internal/net.tcp.client.h"
// task_t
#include "internal/task.h"
// memory_free, memory_counter
#include "internal/memory.h"
// assert
#include "sdk/assert.h"
// window_pooling
#include "internal/window.h"
// math_min
#include "sdk/math.h"

void app_inicialize() {
  app_global.tasks.type = TASK_APPLICATION;
  app_global.tasks.queue.next = &app_global.tasks.queue;
  app_global.tasks.queue.prev = &app_global.tasks.queue;
}

i32 app_run() {
  i64 i = 0;
  task_t* it = &app_global.tasks;
  while (app_global.tasks_count) {
    // process a small fixed number of times to avoid loop starvation.
    for (i = 0; i < 8; i++) {
      task_t* next = (task_t*)it->queue.next;
      switch (it->type) {
        case TASK_APPLICATION:
          window_pooling();
          break;
        // net_tcp_t
        case TASK_TCP_CONNECTING:
          net_tcp_connect_handle((net_tcp_t*)it);
          break;
        case TASK_TCP_WRITING:
          net_tcp_write_handle((net_tcp_t*)it);
          break;
        case TASK_TCP_READING:
          net_tcp_read_handle((net_tcp_t*)it);
          break;
        case TASK_TCP_CLOSING:
          net_tcp_close_handle((net_tcp_t*)it);
          break;
        // net_tcp_server_t
        case TASK_TCP_SERVER_LISTENING:
          net_tcp_server_listen_handle((net_tcp_server_t*)it);
          break;
        case TASK_TCP_SERVER_CLOSING:
          net_tcp_server_close_handle((net_tcp_server_t*)it);
          break;
        // net_tcp_client_t
        case TASK_TCP_CLIENT_WRITING:
          net_tcp_client_write_handle((net_tcp_client_t*)it);
          break;
        case TASK_TCP_CLIENT_READING:
          net_tcp_client_read_handle((net_tcp_client_t*)it);
          break;
        case TASK_TCP_CLIENT_CLOSING:
          net_tcp_client_close_handle((net_tcp_client_t*)it);
          break;
      }
      it = next;
    }
  }
  net_shutdown();
  if (memory_counter > 0) {
    console_warn("memory_counter=%d", memory_counter);
  }
  return 0;
}
