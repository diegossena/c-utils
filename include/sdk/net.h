#ifndef NET_H
#define NET_H

#include <sdk/types.h>
#include <sdk/error.h>


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
    char ip6[16];
  };
} net_address_t;

typedef u64 socket_t;
typedef socket_t tcp_socket_t;
typedef u32 ip4_t;
typedef u16 net_port_t;
typedef char ip4_string_t[16];

export net_address_t ip4_address(u32 ip4, u16 net_port);
export u32 ip4_from_bytes(u8, u8, u8, u8);
export u32 ip4_from_cstr(const char* ip4_cstr);
export void ip4_to_cstr(u32 ip4, ip4_string_t cstr);
export u32 ip4_increment(u32 ip4);
export bool ip4_lessequal(u32 oip4, u32 fip4);
export u16 net_port_from_short(u16 port);
export u16 net_port_to_short(u16 port);

export u64 socket_new(socket_type_t);
export void socket_free(u64 fd);

export error_t net_startup();
export void net_shutdown();

#endif