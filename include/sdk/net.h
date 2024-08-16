#ifndef SDK_NET_H
#define SDK_NET_H

#include <sdk/types.h>
#include <sdk/error.h>
#include <sdk/buffer.h>

#pragma comment(lib, "ws2_32.lib")

#define DEFAULT_TIMEOUT 3000LLU
#define IP4_STRING_LENGTH 16

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
  u32 ip4;
} net_address_t;

typedef char ip4_string_t[IP4_STRING_LENGTH];

extern u64 __net_max_fd;

SDK_EXPORT net_address_t ip4_address(u32 ip4, u16 net_port);
SDK_EXPORT u32 ip4_from_bytes(byte_t, byte_t, byte_t, byte_t);
SDK_EXPORT u32 ip4_from_cstr(const char* ip4_cstr);
SDK_EXPORT void ip4_to_cstr(u32 ip4, ip4_string_t cstr);
SDK_EXPORT u32 ip4_increment(u32 ip4);
SDK_EXPORT bool ip4_lessequal(u32 oip4, u32 fip4);
SDK_EXPORT u16 net_port_from_short(u16 port);
SDK_EXPORT u16 net_port_to_short(u16 port);
SDK_EXPORT void __net_startup();
SDK_EXPORT void __net_shutdown();
SDK_EXPORT u64 socket_new(socket_type_t type);
SDK_EXPORT void __socket_free(u64 fd);

#endif