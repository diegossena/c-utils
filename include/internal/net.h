#pragma once

#include "sdk/net.h"
#include "sdk/types.h"

#define SOCKET_FIELDS \
  u64 socket;

void net_inicialize();
void net_shutdown();

// #define socket_write_cstr(this, cstr)  { \
//   const char ptr[] = cstr;                \
//   socket_write(this, ptr, sizeof(ptr) - 1);  \
// }
// // @returns i32 >= 0 ? bytes_sent : error
// i32 socket_write(net_socket*, const byte* chunk, u32 length);
// // @returns i32 >= 0 ? bytes_received : error
// i32 socket_read(net_socket*, byte* buffer, u32 length);

// class net_socket net_socket;

// error_code socket_connect(net_socket*);