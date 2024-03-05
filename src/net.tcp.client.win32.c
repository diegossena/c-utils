#include "internal/platform.h"

#if PLATFORM_WINDOWS
// queue_remove
#include "internal/net.tcp.client.h"
// app_global
#include "internal/application.h"
// memory_free
#include "internal/memory.h"
// socket, shutdown
#include <winsock2.h>
// TODO: remove this line
#include "sdk/console.h"

void net_tcp_client_close_handle(net_tcp_client_t* this) {
  shutdown(this->socket, FD_WRITE | FD_READ);
  queue_remove(&app_global.tasks, &this->task.queue);
  closesocket(this->socket);
  --this->server->client_count;
  memory_free(this);
}

#endif