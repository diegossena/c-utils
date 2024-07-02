#ifndef SDK_NET_H
#define SDK_NET_H

#include <sdk/types.h>
#include <sdk/error.h>
#include <sdk/buffer.h>

#pragma comment(lib, "ws2_32.lib")

#define DEFAULT_TIMEOUT 5000LLU

typedef struct net_address_t {
  u8 family;
  u16	net_port;
  union {
    u32 ip4;
    char ip6[16];
  };
} net_address_t;

extern u64 __net_max_fd;

SDK_EXPORT void __net_startup();
SDK_EXPORT void __net_shutdown();
SDK_EXPORT u64 __socket_new(i32 family, i32 type, i32 protocol);
SDK_EXPORT void __socket_free(u64 fd);
SDK_EXPORT const char* ip4_to_cstr(u32 ip4);
SDK_EXPORT u32 ip4_increment(u32 ip4);

#endif