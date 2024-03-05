// net_tcp_connect_handle, net_tcp_t
#include "internal/net.tcp.h"
// app_run, app_global
#include "internal/application.h"
// net_tcp_server_listen_handle
#include "internal/net.tcp.server.h"
// task_t
#include "internal/task.h"
// memory_free, memory_counter
#include "internal/memory.h"
// assert
#include "sdk/assert.h"

i32 app_run() {
  task_t* it = (task_t*)app_global.tasks;
  while (it) {
    do {
      task_t* next = (task_t*)it->queue.next;
      switch (it->type) {
        // net_tcp_t
        case TASK_TCP_CONNECTING: net_tcp_connect_handle((net_tcp_t*)it); break;
        case TASK_TCP_WRITING: net_tcp_write_handle((net_tcp_t*)it); break;
        case TASK_TCP_READING: net_tcp_read_handle((net_tcp_t*)it); break;
        case TASK_TCP_CLOSING: net_tcp_close_handle((net_tcp_t*)it); break;
        // net_tcp_server_t
        case TASK_TCP_SERVER_LISTENING:
          net_tcp_server_listen_handle((net_tcp_server_t*)it);
          break;
        case TASK_TCP_SERVER_CLOSING:
          net_tcp_server_close_handle((net_tcp_server_t*)it);
          break;
        // net_tcp_client_t
        case TASK_TCP_CLIENT_WRITING: break;
        case TASK_TCP_CLIENT_READING: break;
        case TASK_TCP_CLIENT_CLOSING: break;
      }
      it = next;
    } while (it);
    it = (task_t*)app_global.tasks;
  }
  net_shutdown();
  assert(memory_counter == 0);
  return 0;
}