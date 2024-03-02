#include "sdk/net.socket.h"

#include "internal/http.server.h"
#include "internal/memory.h"

http_server* http_server_new() {
  http_server* this = memory_alloc0(sizeof(http_server));
  return this;
}
void http_server_free(http_server* this) {
  memory_free(this);
}
