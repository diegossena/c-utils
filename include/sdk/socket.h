#pragma once

#include <sdk/types.h>
#include <sdk/string.h>

/*
```
SocketConstructorOpts options = {
  .host = "localhost",
  .port = 1234,
  .timeout = 0
};
```
*/
interface SocketConstructorOpts {
  string host;
  u16 port, timeout;
} SocketConstructorOpts;

class socket {
  u64 id;
} socket;

void socket_startup();
void socket_shutdown();

void socket_new(socket*, SocketConstructorOpts*);