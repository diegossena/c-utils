#ifndef NET_H
#define NET_H

#include <sdk/types.h>
#include <sdk/error.h>

typedef enum socket_type_t {
  SOCKET_TYPE_STREAM = 1,
  SOCKET_TYPE_DGRAM = 2
} socket_type_t;

typedef enum net_family_t {
  NET_FAMILY_IPV4 = 2,
  NET_FAMILY_IPV6 = 23
} net_family_t;
typedef struct net_address_t {
  u8 family; // net_family_t
  u16	net_port;
  union {
    u32 ip4;
    char ip6[16];
  };
} net_address_t;

typedef u64 socket_t;
typedef u32 ip4_t;
typedef u16 net_port_t;
typedef char ip4_string_t[16];

export ip4_t ip4_from_bytes(u8, u8, u8, u8);
export ip4_t ip4_from_str(const char* ip4_cstr);
export void ip4_to_str(ip4_t ip4, ip4_string_t cstr);
export ip4_t ip4_increment(ip4_t ip4);
export bool ip4_lessequal(ip4_t oip4, ip4_t fip4);

export net_port_t net_port_from_short(u16 port);
export u16 net_port_to_short(net_port_t port);

export u64 _socket_new(socket_type_t);
export void _socket_free(socket_t fd);

export error_t net_startup();
export void net_shutdown();
export error_t net_error();

#endif