#ifndef SDK_NET_H
#define SDK_NET_H

#include <sdk/types.h>

#define DEFAULT_TIMEOUT 10000LLU // 10s

typedef union net_in_addr_t {
  struct { u8  s_b1, s_b2, s_b3, s_b4; } S_un_b;
  struct { u16 s_w1, s_w2; } S_un_w;
  u32 S_addr;
} net_in_addr_t;

typedef struct net_addr_t {
  short	family;
  u16	port;
  net_in_addr_t	in_addr;
  char	sin_zero[8];
} net_addr_t;

u32 net_host_resolve(const char* host);

void net_startup();
/** @brief clear net global library if inicialized
 */
void net_shutdown();

#include <sdk/net/net.win32.inl.h>

#endif