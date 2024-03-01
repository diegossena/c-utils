#pragma once

#include <sdk/types.h>
#include <sdk/error.h>
#include <sdk/string.h>

#define socket_write_cstr(this, cstr)  { \
  const char ptr[] = cstr;                \
  socket_write(this, ptr, sizeof(ptr));  \
}

interface socket_options {
  const char* host;
  u16
    port,
    timeout;
} socket_options;

class net_socket {
  u64 id;
} net_socket;

error_code socket_startup();
void socket_shutdown();

/*
```
socket_constructor(
  &sock, (socket_options) {
  .host = "localhost",
  .port = 8080,
  .timeout = 0
});
```
*/
error_code socket_constructor(net_socket*, socket_options);
void socket_free(net_socket*);

error_code socket_write(net_socket*, const byte* chunk, u32 length);
/*
@returns i32 >= 0 ? bytes_received : error
*/
i32 socket_read(net_socket*, byte* buffer, u32 length);