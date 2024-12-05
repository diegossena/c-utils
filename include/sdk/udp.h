#ifndef UDP_H
#define UDP_H

#include <sdk/types.h>
#include <sdk/net.h>

typedef socket_t udp_t;

export udp_t udp_new();
export void udp_free(udp_t);

export error_t udp_bind(udp_t this, net_port_t port);
export i32 udp_read(udp_t this, char* target, u64 length, net_address_t* address);
export i32 udp_send(udp_t this, const char* buffer, u64 length, net_address_t* address);

#endif