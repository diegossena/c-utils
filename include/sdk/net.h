#ifndef NET_H
#define NET_H

#include <sdk/types.h>
#include <sdk/error.h>

typedef enum socket_type_t {
  SOCKET_TYPE_STREAM = 1,
  SOCKET_TYPE_DGRAM = 2
} socket_type_t;

typedef u64 socket_t;
typedef u32 ip4_t;
typedef u16 net_port_t;
typedef char ip4_string_t[16];

export u32 ip4_from_bytes(u8, u8, u8, u8);
export u32 ip4_from_str(const char* ip4_cstr);
export void ip4_to_str(u32 ip4, ip4_string_t cstr);
export u32 ip4_increment(u32 ip4);
export bool ip4_lessequal(u32 oip4, u32 fip4);

export u16 net_port_from_short(u16 port);
export u16 net_port_to_short(u16 port);

export u64 _socket_new(socket_type_t);
export void _socket_free(u64 fd);

export error_t net_startup();
export void net_shutdown();

#endif