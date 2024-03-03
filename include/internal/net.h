#pragma once

#include "sdk/net.h"
#include "sdk/types.h"

typedef union net_in_addr_t {
  struct { u8  s_b1, s_b2, s_b3, s_b4; } S_un_b;
  struct { u16 s_w1, s_w2; } S_un_w;
  u32 S_addr;
} net_in_addr_t;

interface net_addr_t {
  short	family;
  u16	sin_port;
  net_in_addr_t	in_addr;
  char	sin_zero[8];
} net_addr_t;

/** @brief clear net global library if inicialized
 */
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