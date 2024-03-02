#pragma once

#include <sdk/types.h>
#include <sdk/error.h>
#include <sdk/string.h>

#define socket_write_cstr(this, cstr)  { \
  const char ptr[] = cstr;                \
  socket_write(this, ptr, sizeof(ptr) - 1);  \
}

class net_socket net_socket;

error_code socket_startup();
void socket_shutdown();

net_socket* socket_new();
void socket_free(net_socket*);

void socket_host_set(net_socket*, const char*);
void socket_port_set(net_socket*, u64);

error_code socket_connect(net_socket*);

// @returns i32 >= 0 ? bytes_sent : error
i32 socket_write(net_socket*, const byte* chunk, u32 length);
// @returns i32 >= 0 ? bytes_received : error
i32 socket_read(net_socket*, byte* buffer, u32 length);