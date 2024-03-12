#include <sdk/application/application.h>

#include <sdk/net/net.tcp.h>
#include <sdk/net/net.tcp.server.h>
#include <sdk/net/net.tcp.client.h>
#include <sdk/application/task.h>
#include <sdk/memory/memory.h>
#include <sdk/window/window.h>
#include <sdk/assert.h>

void app_constructor(application_t* this) {
  this->__tasks.type = TASK_MAIN;
  this->__tasks.queue.next = (queue_t*)&this->__tasks;
  this->__tasks.queue.prev = (queue_t*)&this->__tasks;
}

i32 app_run(application_t* this) {
  task_t* it = (task_t*)this->__tasks.queue.next;
  while (true) {
    task_t* next = (task_t*)it->queue.next;
    switch (it->type) {
      case TASK_MAIN:
        if (&it->queue == it->queue.next)
          return 0;
        break;
      case TASK_WINDOW:
        window_pooling();
        break;
      // net_tcp_server_t
      case TASK_TCP_SERVER_LISTENING:
        net_tcp_server_listen_handle((net_tcp_server_t*)it);
        break;
      case TASK_TCP_SERVER_CLOSING:
        net_tcp_server_close_handle((net_tcp_server_t*)it);
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
        net_tcp_free((net_tcp_t*)it);
        break;
      // net_tcp_client_t
      case TASK_TCP_CLIENT_WRITING:
        net_tcp_client_write_handle((net_tcp_client_t*)it);
        break;
      case TASK_TCP_CLIENT_READING:
        net_tcp_client_read_handle((net_tcp_client_t*)it);
        break;
      case TASK_TCP_CLIENT_CLOSING:
        net_tcp_client_free((net_tcp_client_t*)it);
        break;
    }
    it = next;
  }
  net_shutdown();
  if (memory_counter > 0) {
    console_warn("memory_counter=%d", memory_counter);
  }
  return 0;
}