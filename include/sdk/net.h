#ifndef SDK_NET_H
#define SDK_NET_H

#include <sdk/types.h>
#include <sdk/error.h>
#include <sdk/buffer.h>

#pragma comment(lib, "ws2_32.lib")

#define DEFAULT_TIMEOUT 5000LLU

typedef enum net_family_t {
  NET_FAMILY_IPV4 = 2,
  NET_FAMILY_IPV6 = 23
} net_family_t;
typedef enum socket_type_t {
  SOCKET_TYPE_STREAM = 1,
  SOCKET_TYPE_DGRAM = 2
} socket_type_t;


typedef struct net_address_t {
  u8 family; // net_family_t
  u16	net_port;
  union {
    u32 ip4;
    u8 ip6[16];
  };
} net_address_t;

extern u64 __net_max_fd;

SDK_EXPORT const char* ip4_to_cstr(u32 ip4);
SDK_EXPORT u32 ip4_increment(u32 ip4);
SDK_EXPORT void __net_startup();
SDK_EXPORT void __net_shutdown();
SDK_EXPORT u64 __socket_new(net_family_t family, socket_type_t type);
SDK_EXPORT void __socket_free(u64 fd);
SDK_EXPORT u16 net_port_to_short(u16 port);

#endif