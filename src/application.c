#include "internal/console.h"
#include "internal/net.h"
#include "internal/application.h"
#include "sdk/application.h"
#include "internal/handle.h"
#include "internal/net.tcp.h"

void app_inicialize() {
  console_inicialize();
  net_inicialize();
}

void app_shutdown() {
  net_shutdown();
}

void app_run() {
  while (!app_main.stop_flag) {
    handle* it = (handle*)app_main.app_queue_head;
    while (it) {
      switch (it->type) {
        case HANDLE_TCP_LISTEN:
          net_tcp_listen_handle((net_tcp*)it);
          break;
      }
      it = (handle*)it->handle_queue.next;
    }
  }
  app_shutdown();
}