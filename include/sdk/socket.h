#pragma once

#include <sdk/types.h>
#include <sdk/error.h>
#include <sdk/string.h>

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
void socket_constructor(net_socket*, socket_options);