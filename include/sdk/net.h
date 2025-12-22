#ifndef NET_H
#define NET_H

#include <sdk/types.h>
#include <sdk/error.h>

typedef enum socket_type_t {
  SOCKET_STREAM = 1,
  SOCKET_DGRAM = 2
} socket_type_t;

typedef enum net_family_t {
  NET_FAMILY_IPV4 = 2,
  NET_FAMILY_IPV6 = 23
} net_family_t;

typedef struct net_address_t {
  u8 family; // net_family_t
  u16	net_port;
  u32 ip4;
  char _pad[5];
} net_address_t;

typedef u64 socket_t;
typedef u32 ip4_t;

void net_startup();
inline void net_shutdown();

inline u16 net_port_short(u16 port);

inline socket_t _socket_new(socket_type_t);
inline void _socket_free(socket_t fd);

inline ip4_t ip4_from_bytes(u8, u8, u8, u8);
ip4_t ip4_from_str(const char* ip4_cstr);
void ip4_to_str(ip4_t ip4, char* out);
ip4_t ip4_increment(ip4_t ip4);
bool ip4_lessequal(ip4_t oip4, ip4_t fip4);


#endif