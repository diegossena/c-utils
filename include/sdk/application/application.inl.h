#include <sdk/application/application.h>
#include <sdk/memory/memory.h>
#include <sdk/assert.h>
#ifdef SDK_WINDOW_H
#include <sdk/window/window.h>
#endif
#ifdef SDK_NET_TCP_H
#include <sdk/net/net.tcp.h>
#endif
#ifdef SDK_NET_TCP_SERVER_H
#include <sdk/net/tcp/server.h>
#endif

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
#ifdef SDK_WINDOW_H
      case TASK_WINDOW:
        window_pooling();
        break;
#endif      
#ifdef SDK_NET_TCP_SERVER_H
      case TASK_TCP_SERVER_LISTENING:
        net_tcp_server_listen_handle((net_tcp_server_t*)it);
        break;
      case TASK_TCP_SERVER_CLOSING:
        net_tcp_server_close_handle((net_tcp_server_t*)it);
        break;
      case TASK_TCP_CLIENT_WRITING:
        net_tcp_client_write_handle((net_tcp_client_t*)it);
        break;
      case TASK_TCP_CLIENT_READING:
        net_tcp_client_read_handle((net_tcp_client_t*)it);
        break;
      case TASK_TCP_CLIENT_CLOSING:
        net_tcp_client_free((net_tcp_client_t*)it);
        break;
#endif
#ifdef SDK_NET_TCP_H
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
#endif
    }
    it = next;
  }
#ifdef SDK_NET_H
  net_shutdown();
#endif
  if (memory_counter > 0) {
    console_warn("memory_counter=%d", memory_counter);
  }
  return 0;
}