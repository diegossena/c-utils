#include "internal/console.h"
#include "internal/net.h"
#include "internal/application.h"
#include "sdk/application.h"
#include "internal/handle.h"
#include "internal/net.tcp.h"

i32 app_run() {
  while (!app_global.stop_flag) {
    handle* it = (handle*)app_global.app_queue_head;
    while (it) {
      switch (it->type) {
        case HANDLE_TCP_LISTEN:
          net_tcp_listen_handle((net_tcp*)it);
          break;
      }
      it = (handle*)it->handle_queue.next;
    }
  }
  net_shutdown();
  return 0;
}