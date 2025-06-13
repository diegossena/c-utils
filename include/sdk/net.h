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

ip4_t ip4_from_bytes(u8, u8, u8, u8);
ip4_t ip4_from_str(const char* ip4_cstr);
void ip4_to_str(ip4_t ip4, ip4_string_t cstr);
ip4_t ip4_increment(ip4_t ip4);
bool ip4_lessequal(ip4_t oip4, ip4_t fip4);

net_port_t net_port_from_short(u16 port);
u16 net_port_to_short(net_port_t port);

error_t net_startup();
void net_shutdown();
error_t net_error();

u64 _socket_new(socket_type_t);
void _socket_free(socket_t fd);

#ifdef DEBUG

extern bool __net_started;
extern i64 __socket_count;
#define __socket_count_increment() ++__socket_count
#define __socket_count_decrement() --__socket_count

#else

#define __socket_count_increment(this)
#define __socket_count_decrement()

#endif

#endif